#ifndef BDEV_H
#define BDEV_H

/* util.c */
void lk2nd_bdev_dump_devices(void);

void lk2nd_wrapper_bio_register(void);
void lk2nd_mmc_sdhci_bio_register(void);

#endif
