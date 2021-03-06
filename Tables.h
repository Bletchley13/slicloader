/*
 *  SLICLoader, an EFI program to modify ACPI tables before boot.
 *  Copyright (C) 2013 wweber
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SL_TABLES_H_
#define SL_TABLES_H_

#include <Uefi.h>
#include <Library/UefiLib.h>

/**
 * @file Tables.h
 *
 * Contains functions that work with ACPI tables.
 */

/**
 * The maximum number of tables in the stack.
 */
#define TABLE_STACK_SIZE 256

/**
 * This struct keeps track of ACPI tables.
 *
 * It should be able to dynamically expand, but I'm lazy.
 */
typedef struct _TABLE_STACK
{
	void* Tables[TABLE_STACK_SIZE];
	UINTN TableCount;
} TABLE_STACK;

/**
 * Structure of the ACPI RSDP table.
 */
typedef struct _RSDP_TABLE
{
	CHAR8 Signature[8];
	UINT8 Checksum;
	CHAR8 OEMID[6];
	UINT8 Revision;
	UINT32 RSDT;
	UINT32 Length;
	UINT64 XSDT;
	UINT8 ExtendedChecksum;
} RSDP_TABLE;

/**
 * Structure of the ACPI System Description table header
 */
typedef struct _SD_HEADER
{
	CHAR8 Signature[4];
	UINT32 Length;
	UINT8 Revision;
	UINT8 Checksum;
	CHAR8 OEMID[6];
	CHAR8 OEMTableID[8];
	UINT32 OEMRevision;
	CHAR8 CreatorID[4];
	CHAR8 CreatorRevision[4];
} SD_HEADER;
//36 bytes, 0-35

/**
 * Size of the system description table header
 */
#define SD_HEADER_SIZE 36

/**
 * Casts a UINT32 to a SD_HEADER*
 * @param i
 * @return
 */
SD_HEADER* Int32ToTable(UINT32 i);

/**
 * Casts a UINT64 to a SD_HEADER*
 * @param i
 * @return
 */
SD_HEADER* Int64ToTable(UINT64 i);

/**
 * Gets a pointer to the RSDT table
 * @param rsdp The RSDP table
 * @return
 */
SD_HEADER* GetRSDT(RSDP_TABLE* rsdp);

/**
 * Gets a pointer tot he XSDT table
 * @param rsdp The RSDP table
 * @return
 */
SD_HEADER* GetXSDT(RSDP_TABLE* rsdp);

/**
 * Returns an array of pointers (as uint32) to RSDT tables
 * @param rsdt The RSDT table
 * @param[out] count Sets the number of tables
 * @return
 */
UINT32* GetRSDTTables(SD_HEADER* rsdt, UINTN* count);

/**
 * Returs an array of poitners (as uint64) to XSDT tables.
 * @param xsdt The XSDT table
 * @param[out] count The number of tables
 * @return
 */
UINT64* GetXSDTTables(SD_HEADER* xsdt, UINTN* count);

/**
 * Calculates the checksum for a block of data.
 * @param data Pointer to the data to sum
 * @param count Length in bytes of the data to sum
 * @param exclude Offset from data of a byte to exclude from the sum
 * @return The sum
 */
UINT8 GetChecksum(void* data, UINTN count, UINTN exclude);

/**
 * Compares a table's signature to a string.
 * @param table The table to compare
 * @param sig The signature. Must be a 4-byte 0-terminated string, so 5 bytes in total.
 * @return True if it matches, false otherwise.
 */
BOOLEAN IsTableType(SD_HEADER* table, CHAR8* sig);

//Actual do-stuff functions

/**
 * Searches for all tables via the RSDP, and adds them to a table stack
 * @param rsdp The RSDP table pointer.
 * @param stack Pointer to the TABLE_STACK object to fill.
 */
void FindAllTables(RSDP_TABLE* rsdp, TABLE_STACK* stack);

/**
 * Patches the ACPI tables, replacing the SLIC and OEM ids
 * @param ImageHandle The loaded image handle
 * @param systab The system table pointer
 * @param slic_data Pointer to the new SLIC data
 * @param slic_size Size of the new SLIC
 */
EFI_STATUS PatchTables(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* systab,
		void* slic_data, UINTN slic_size);

#endif
