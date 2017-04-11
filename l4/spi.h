#pragma once
#include <stdint.h>
#include <stddef.h>

enum spi_br {
	SPI_BR_2,
	SPI_BR_4,
	SPI_BR_8,
	SPI_BR_16,
	SPI_BR_32,
	SPI_BR_64,
	SPI_BR_128,
	SPI_BR_256,
};
struct spi_cr1 {
	uint32_t cpha : 1, cpol : 1, mstr : 1;
	enum spi_br br : 3;
	uint32_t spe : 1, lsbfirst : 1, ssi : 1, ssm : 1, rxonly : 1, crcl : 1,
		 crcnext : 1, crcen : 1, bidioe : 1, bidimode : 1, : 16;
};

enum spi_ds {
	SPI_DS_4 = 3,
	SPI_DS_5,
	SPI_DS_6,
	SPI_DS_7,
	SPI_DS_8,
	SPI_DS_9,
	SPI_DS_10,
	SPI_DS_11,
	SPI_DS_12,
	SPI_DS_13,
	SPI_DS_14,
	SPI_DS_15,
	SPI_DS_16,
};
_Static_assert (SPI_DS_16 == 15, "SPI_DS_n have gone bad.");
struct spi_cr2 {
	uint32_t rxdmaen : 1, txdmaen : 1, ssoe : 1, nssp : 1, frf : 1,
		 errie : 1, rxneie : 1, txeie : 1;
	enum spi_ds ds : 4;
	uint32_t frxth : 1, ldma_rx : 1, ldma_tx : 1, : 1, : 16;
};

struct spi_sr {
	uint32_t rxne : 1, txe : 1, : 2, crcerr : 1, modf : 1, ovr : 1, bsy : 1,
		 fre : 1, frlvl : 2, ftlvl : 2, : 3, : 16;
};

struct spi_dr {
	uint32_t dr : 16, : 16;
};

struct spi_crcpr {
	uint32_t crcpoly : 16, : 16;
};

struct spi_rxcrcr {
	uint32_t rxcrc : 16, : 16;
};

struct spi_txcrcr {
	uint32_t txcrc : 16, : 16;
};

struct spi_reg {
	struct spi_cr1 cr1;		// 0x00
	struct spi_cr2 cr2;		// 0x04
	struct spi_sr sr;		// 0x08
	struct spi_dr dr;		// 0x0C
	struct spi_crcpr crcpr;		// 0x10
	struct spi_rxcrcr rxcrcr;	// 0x14
	struct spi_txcrcr txcrcr;	// 0x18
};

_Static_assert (offsetof(struct spi_reg, txcrcr) == 0x18,
		"SPI registers have gone bad.");
