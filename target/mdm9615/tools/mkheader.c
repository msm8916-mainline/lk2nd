/*
 * Copyright (c) 2007, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2011, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/stat.h>

int print_usage(){
	fprintf(stderr,"usage: mkheader <bin> <hdr> <none|unified-boot>\n");
	fprintf(stderr,"       mkheader <bin> <hdr> <unsecure-boot>"
		       " <outbin>\n");
	fprintf(stderr,"       mkheader <bin> <hdr> <secure-boot> <outbin>"
		       " <maxsize>\n");
	fprintf(stderr,"       mkheader <bin> <hdr> <secure-boot> <outbin>"
		       " <maxsize> <certchain> <files...>\n\n");
	fprintf(stderr,"bin:               Input raw appsbl binary\n");
	fprintf(stderr,"hdr:               Output of appsbl header location\n");
	fprintf(stderr,"outbin:            Output of the signed or unsigned"
		       " apps boot location\n");
	fprintf(stderr,"maxsize:           Maximum size for certificate"
		       " chain\n");
	fprintf(stderr,"certchain:         Output of the certchain location\n");
	fprintf(stderr,"files:             Input format <bin signature>"
		       " <certifcate file(s) for certificate chain>...\n");
	fprintf(stderr,"certificate chain: Files will be concatenated in order"
		       " to create the certificate chain\n\n");
	return -1;
}

int cat(FILE * in, FILE * out, unsigned size, unsigned buff_size){
	unsigned bytes_left = size;
	char buf[buff_size];
	int ret = 0;

	while(bytes_left){
		fread(buf, sizeof(char), buff_size, in);
		if(!feof(in)){
			bytes_left -= fwrite(buf, sizeof(char), buff_size, out);
		}
		else
			bytes_left = 0;
	}
	ret = ferror(in) | ferror(out);
	if(ret)
		fprintf(stderr, "ERROR: Occured during file concatenation\n");
	return ret;
}

int main(int argc, char *argv[])
{
	struct stat s;
	unsigned size, base;
	int unified_boot = 0;
	unsigned unified_boot_magic[20];
	unsigned non_unified_boot_magic[10];
	unsigned magic_len = 0;
	unsigned *magic;
	unsigned cert_chain_size = 0;
	unsigned signature_size = 0;
	int secure_boot = 0;
	int fd;

	if(argc < 3) {
		return print_usage();
	}

	if(argc == 4) {
		if(!strcmp("unified-boot",argv[3])) {
			unified_boot = 1;
		}
		else if(!strcmp("secure-boot",argv[3])){
			fprintf(stderr,
				"ERROR: Missing arguments: [outbin maxsize] |"
				" [outbin, maxsize, certchain,"
				" signature + certifcate(s)]\n");
			return print_usage();
		}
		else if(!strcmp("unsecure-boot",argv[3])){
			fprintf(stderr,"ERROR: Missing arguments:"
				       " outbin directory\n");
			return print_usage();
		}
	}

	if(argc > 4) {
		if(!strcmp("secure-boot",argv[3])) {
			if(argc < 9 && argc != 6){
				fprintf(stderr,
					"ERROR: Missing argument(s):"
					" [outbin maxsize] | [outbin, maxsize,"
					" certchain,"
					" signature + certifcate(s)]\n");
				return print_usage();
			}
			secure_boot = 1;
			signature_size = 256; //Support SHA 256
			cert_chain_size = atoi(argv[5]);
		}
	}

	if(stat(argv[1], &s)) {
		perror("cannot stat binary");
		return -1;
	}

	if(unified_boot) {
		magic = unified_boot_magic;
		magic_len = sizeof(unified_boot_magic);
	}
	else {
		magic = non_unified_boot_magic;
		magic_len = sizeof(non_unified_boot_magic);
	}

	size = s.st_size;
#if MEMBASE
	base = MEMBASE;
#else
	base = 0;
#endif

	printf("Image Destination Pointer: 0x%x\n", base);

	magic[0] = 0x00000005; /* appsbl */
	magic[1] = 0x00000003; //Flash_partition_version /* nand */
	magic[2] = 0x00000000; //image source pointer
	magic[3] = base;       //image destination pointer
	magic[4] = size + cert_chain_size + signature_size; //image size
	magic[5] = size;       //code size
	magic[6] = base + size;
	magic[7] = signature_size;
	magic[8] = size + base + signature_size;
	magic[9] = cert_chain_size;

	if(unified_boot == 1)
	{
		magic[10] = 0x33836685;	/* cookie magic number */
		magic[11] = 0x00000001;	/* cookie version */
		magic[12] = 0x00000002;	/* file formats */
		magic[13] = 0x00000000;
		magic[14] = 0x00000000;	/* not setting size for boot.img */
		magic[15] = 0x00000000;
		magic[16] = 0x00000000;
		magic[17] = 0x00000000;
		magic[18] = 0x00000000;
		magic[19] = 0x00000000;
	}

	fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(fd < 0) {
		perror("cannot open header for writing");
		return -1;
	}
	if(write(fd, magic, magic_len) != magic_len) {
		perror("cannot write header");
		close(fd);
		unlink(argv[2]);
		return -1;
	}
	close(fd);

	if(secure_boot && argc > 6){
		FILE * input_file;
		FILE * output_file;
		unsigned buff_size = 1;
		char buf[buff_size];
		unsigned bytes_left;
		unsigned current_cert_chain_size = 0;
		int padding_size = 0;
		int i;

		if((output_file = fopen(argv[6], "wb"))==NULL){
			perror("ERROR: Occured during fopen");
			return -1;
		}
		printf("Certificate Chain Output File: %s\n", argv[6]);

		for(i = 8; i < argc; i++){
			if((input_file = fopen(argv[i], "rb"))==NULL){
				perror("ERROR: Occured during fopen");
				return -1;
			}
			stat(argv[i], &s);
			bytes_left = s.st_size;
			current_cert_chain_size += bytes_left;
			if(cat(input_file, output_file, bytes_left, buff_size))
				return -1;
			fclose(input_file);
		}

		//Pad certifcate chain to the max expected size from input
		memset(buf, 0xFF, sizeof(buf));
		padding_size = cert_chain_size - current_cert_chain_size;

		if(padding_size <0){
			fprintf(stderr, "ERROR: Input certificate chain"
					" (Size=%d) is larger than the maximum"
					" specified (Size=%d)\n",
				current_cert_chain_size, cert_chain_size);
			return -1;
		}

		bytes_left = (padding_size > 0) ? padding_size : 0;
		while(bytes_left){
			if(!ferror(output_file))
				bytes_left -= fwrite(buf,
						     sizeof(buf),
						     buff_size,
						     output_file);
			else{
				fprintf(stderr, "ERROR: Occured during"
						" certifcate chain padding\n");
				return -1;
			}
		}
		fclose(output_file);

		/* Concat and combine to signed image.
		 * Format [HDR][RAW APPSBOOT][PADDED CERT CHAIN]
		 */
		if((output_file = fopen(argv[4], "wb"))==NULL){
			perror("ERROR: Occured during fopen");
			return -1;
		}
		printf("Image Output File: %s\n", argv[4]);

		//Header
		if((input_file = fopen(argv[2], "rb"))==NULL){
			perror("ERROR: Occured during fopen");
			return -1;
		}
		stat(argv[2], &s);
		if(cat(input_file, output_file, s.st_size, buff_size))
			return -1;
		fclose(input_file);

		//Raw Appsbl
		if((input_file = fopen(argv[1], "rb"))==NULL){
			perror("ERROR: Occured during fopen");
			return -1;
		}
		stat(argv[1], &s);
		if(cat(input_file, output_file, s.st_size, buff_size))
			return -1;
		fclose(input_file);

		//Signature
		if((input_file = fopen(argv[7], "rb"))==NULL){
			perror("ERROR: Occured during fopen");
			return -1;
		}
		stat(argv[7], &s);
		if(cat(input_file, output_file, s.st_size, buff_size))
			return -1;
		fclose(input_file);

		//Certifcate Chain
		if((input_file = fopen(argv[6], "rb"))==NULL){
			perror("ERROR: Occured during fopen");
			return -1;
		}
		if(cat(input_file, output_file,
		       (current_cert_chain_size + padding_size), buff_size))
			return -1;
		fclose(input_file);

		fclose(output_file);

	}
	else if(argc == 5 || argc == 6){
		FILE * input_file;
		FILE * output_file;
		unsigned buff_size = 1;
		char buf[buff_size];

		/* Concat and combine to unsigned image.
		 * Format [HDR][RAW APPSBOOT]
		 */
		if((output_file = fopen(argv[4], "wb"))==NULL){
			perror("ERROR: Occured during fopen");
			return -1;
		}
		printf("Image Output File: %s\n", argv[4]);

		//Header
		if((input_file = fopen(argv[2], "rb"))==NULL){
			perror("ERROR: Occured during fopen");
			return -1;
		}
		stat(argv[2], &s);
		if(cat(input_file, output_file, s.st_size, buff_size))
			return -1;
		fclose(input_file);

		//Raw Appsbl
		if((input_file = fopen(argv[1], "rb"))==NULL){
			perror("ERROR: Occured during fopen");
			return -1;
		}
		stat(argv[1], &s);
		if(cat(input_file, output_file, s.st_size, buff_size))
			return -1;
		fclose(input_file);
		fclose(output_file);
	}

	printf("Done execution\n");

	return 0;
}
