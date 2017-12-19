#ifndef _LIBFDT_ENV_H
#define _LIBFDT_ENV_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <km_main.h>

typedef uint32_t fdt32_t;

#define EXTRACT_BYTE(n)	((unsigned long long)((uint8_t *)&x)[n])
static inline uint16_t fdt16_to_cpu(uint16_t x)
{
	return (EXTRACT_BYTE(0) << 8) | EXTRACT_BYTE(1);
}
#define cpu_to_fdt16(x) fdt16_to_cpu(x)

static inline uint32_t fdt32_to_cpu(uint32_t x)
{
	return (EXTRACT_BYTE(0) << 24) | (EXTRACT_BYTE(1) << 16) | (EXTRACT_BYTE(2) << 8) | EXTRACT_BYTE(3);
}
#define cpu_to_fdt32(x) fdt32_to_cpu(x)

static inline uint64_t fdt64_to_cpu(uint64_t x)
{
	return (EXTRACT_BYTE(0) << 56) | (EXTRACT_BYTE(1) << 48) | (EXTRACT_BYTE(2) << 40) | (EXTRACT_BYTE(3) << 32)
		| (EXTRACT_BYTE(4) << 24) | (EXTRACT_BYTE(5) << 16) | (EXTRACT_BYTE(6) << 8) | EXTRACT_BYTE(7);
}
#define cpu_to_fdt64(x) fdt64_to_cpu(x)
#undef EXTRACT_BYTE

#define toupper(a)  ((((a) >= 'a') && ((a) <= 'z')) ? ((a) - 'a' + 'A') : (a))
#define isalpha(chr) (('a' <= chr && chr <= 'z') || ('A' <= chr && chr <= 'Z'))

static bool isalnum(unsigned char Chr)
{
        return (('0' <= Chr && Chr <= '9') ||
                ('A' <= Chr && Chr <= 'Z') ||
                ('a' <= Chr && Chr <= 'z')
                );
}

static int Digit2Val( int c)
{
        if(isalpha(c)) {  /* If c is one of [A-Za-z]... */
                c = toupper(c) - 7;   // Adjust so 'A' is ('9' + 1)
        }
        return c - '0';   // Value returned is between 0 and 35, inclusive.
}


static inline int isspace (int c)
{
  //
  // <space> ::= [ ]
  //
        return ((c) == ' ');
}

/** The strtoul function converts the initial portion of the string pointed to
    by nptr to unsigned long int representation.

    See the description for strtol for more information.

  @return   The strtoul function returns the converted value, if any. If no
            conversion could be performed, zero is returned. If the correct
            value is outside the range of representable values, ULONG_MAX is
            returned and the value of the macro ERANGE is stored in errno.
**/
static inline unsigned long
strtoul(const char * __restrict nptr, char ** __restrict endptr, int base)
{
        const char     *pEnd;
        unsigned long   Result = 0;
        unsigned long   Previous;
        int             temp;

        pEnd = nptr;

        if((base < 0) || (base == 1) || (base > 36)) {
                if(endptr != NULL) {
                        *endptr = NULL;
                }
                return 0;
        }
// Skip leading spaces.
        while(isspace(*nptr))   ++nptr;

// Process Subject sequence: optional + sign followed by digits.
        if(*nptr == '+') {
                ++nptr;
        }

        if(*nptr == '0') {  /* Might be Octal or Hex */
                if(toupper(nptr[1]) == 'X') {   /* Looks like Hex */
                        if((base == 0) || (base == 16)) {
                                nptr += 2;  /* Skip the "0X"      */
                                base = 16;  /* In case base was 0 */
                        }
                }
                else {    /* Looks like Octal */
                        if((base == 0) || (base == 8)) {
                                ++nptr;     /* Skip the leading "0" */
                                base = 8;   /* In case base was 0   */
                        }
                }
        }
        if(base == 0) {   /* If still zero then must be decimal */
                base = 10;

	}
        if(*nptr  == '0') {
                for( ; *nptr == '0'; ++nptr);  /* Skip any remaining leading zeros */
                pEnd = nptr;
	}

        while( isalnum(*nptr) && ((temp = Digit2Val(*nptr)) < base)) {
                Previous = Result;
                Result = (Result * base) + (unsigned long)temp;
                if( Result < Previous)  {   // If we overflowed
                        Result = UINT32_MAX;
                        //errno = -1;
                        break;
                }
                pEnd = ++nptr;
        }
// Save pointer to final sequence
        if(endptr != NULL) {
                *endptr = (char *)pEnd;
        }
        return Result;
}

#endif /* _LIBFDT_ENV_H */
