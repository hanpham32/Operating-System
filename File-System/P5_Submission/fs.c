// ============================================================================
// fs.c - user FileSytem API
// ============================================================================

#include "bfs.h"
#include "fs.h"
#include <stdbool.h>

// ============================================================================
// Close the file currently open on file descriptor 'fd'.
// ============================================================================
i32 fsClose(i32 fd)
{
  i32 inum = bfsFdToInum(fd);
  bfsDerefOFT(inum);
  return 0;
}

// ============================================================================
// Create the file called 'fname'.  Overwrite, if it already exsists.
// On success, return its file descriptor.  On failure, EFNF
// ============================================================================
i32 fsCreate(str fname)
{
  i32 inum = bfsCreateFile(fname);
  if (inum == EFNF)
    return EFNF;
  return bfsInumToFd(inum);
}

// ============================================================================
// Format the BFS disk by initializing the SuperBlock, Inodes, Directory and
// Freelist.  On succes, return 0.  On failure, abort
// ============================================================================
i32 fsFormat()
{
  FILE *fp = fopen(BFSDISK, "w+b");
  if (fp == NULL)
    FATAL(EDISKCREATE);

  i32 ret = bfsInitSuper(fp); // initialize Super block
  if (ret != 0)
  {
    fclose(fp);
    FATAL(ret);
  }

  ret = bfsInitInodes(fp); // initialize Inodes block
  if (ret != 0)
  {
    fclose(fp);
    FATAL(ret);
  }

  ret = bfsInitDir(fp); // initialize Dir block
  if (ret != 0)
  {
    fclose(fp);
    FATAL(ret);
  }

  ret = bfsInitFreeList(); // initialize Freelist
  if (ret != 0)
  {
    fclose(fp);
    FATAL(ret);
  }

  ret = bfsInitOFT(); // initialize OFT
  if (ret != 0)
  {
    fclose(fp);
    FATAL(ret);
  }

  fclose(fp);
  return 0;
}

// ============================================================================
// Mount the BFS disk.  It must already exist
// ============================================================================
i32 fsMount()
{
  FILE *fp = fopen(BFSDISK, "rb");
  if (fp == NULL)
    FATAL(ENODISK); // BFSDISK not found
  fclose(fp);
  return 0;
}

// ============================================================================
// Open the existing file called 'fname'.  On success, return its file
// descriptor.  On failure, return EFNF
// ============================================================================
i32 fsOpen(str fname)
{
  i32 inum = bfsLookupFile(fname); // lookup 'fname' in Directory
  if (inum == EFNF)
    return EFNF;
  return bfsInumToFd(inum);
}

// ============================================================================
// Read 'numb' bytes of data from the cursor in the file currently fsOpen'd on
// File Descriptor 'fd' into 'buf'.  On success, return actual number of bytes
// read (may be less than 'numb' if we hit EOF).  On failure, abort
// ============================================================================
i32 fsRead(i32 fd, i32 numb, void *buf)
{

  i32 inum = bfsFdToInum(fd); // retrieve inode

  i32 current_pointer = bfsTell(fd); // get current pointer pos within file

  // calculate block range for reading
  i32 start_file_block = current_pointer / BYTESPERBLOCK;
  i32 end_file_block = (current_pointer + numb) / BYTESPERBLOCK;

  i32 byte_read = numb;
  i32 file_size = bfsGetSize(inum);

  // adjusting read length for file size
  if (current_pointer + numb > file_size)
  {
    byte_read = file_size - current_pointer;                        // set actual number of bytes to read
    end_file_block = (current_pointer + byte_read) / BYTESPERBLOCK; // recalculate the end of file block based on adjusted byte count
  }

  i8 total_blocks = end_file_block - start_file_block + 1; // total number of blocks to read

  // buffer allocation
  i8 read_buff[(total_blocks)*BYTESPERBLOCK];
  i8 loop_buff[BYTESPERBLOCK]; // for reading individual blocks
  i32 buff_offset = 0;

  // read data into buffer
  for (i32 i = start_file_block; i <= end_file_block; i++)
  {
    bfsRead(inum, i, loop_buff);                                 // read a block file the file
    memcpy((read_buff + buff_offset), loop_buff, BYTESPERBLOCK); // copy the read block into buffer
    buff_offset += BYTESPERBLOCK;                                // increment offset for the next block
  }

  // copy data into buffer
  memcpy(buf, (read_buff + (current_pointer % BYTESPERBLOCK)), byte_read);

  // update the file current's pointer pos
  fsSeek(fd, byte_read, SEEK_CUR);
  return byte_read;
}

// ============================================================================
// Move the cursor for the file currently open on File Descriptor 'fd' to the
// byte-offset 'offset'.  'whence' can be any of:
//
//  SEEK_SET : set cursor to 'offset'
//  SEEK_CUR : add 'offset' to the current cursor
//  SEEK_END : add 'offset' to the size of the file
//
// On success, return 0.  On failure, abort
// ============================================================================
i32 fsSeek(i32 fd, i32 offset, i32 whence)
{

  if (offset < 0)
    FATAL(EBADCURS);

  i32 inum = bfsFdToInum(fd);
  i32 ofte = bfsFindOFTE(inum);

  switch (whence)
  {
  case SEEK_SET:
    g_oft[ofte].curs = offset;
    break;
  case SEEK_CUR:
    g_oft[ofte].curs += offset;
    break;
  case SEEK_END:
  {
    i32 end = fsSize(fd);
    g_oft[ofte].curs = end + offset;
    break;
  }
  default:
    FATAL(EBADWHENCE);
  }
  return 0;
}

// ============================================================================
// Return the cursor position for the file open on File Descriptor 'fd'
// ============================================================================
i32 fsTell(i32 fd)
{
  return bfsTell(fd);
}

// ============================================================================
// Retrieve the current file size in bytes.  This depends on the highest offset
// written to the file, or the highest offset set with the fsSeek function.  On
// success, return the file size.  On failure, abort
// ============================================================================
i32 fsSize(i32 fd)
{
  i32 inum = bfsFdToInum(fd);
  return bfsGetSize(inum);
}

// ============================================================================
// Write 'numb' bytes of data from 'buf' into the file currently fsOpen'd on
// filedescriptor 'fd'.  The write starts at the current file offset for the
// destination file.  On success, return 0.  On failure, abort
// ============================================================================
i32 fsWrite(i32 fd, i32 numb, void *buf)
{

  i32 inum = bfsFdToInum(fd); // retrieve inode from file descriptor

  i32 current_pointer = bfsTell(fd); // get current pointer where write will start

  // calculate starting and ending blocks
  i32 start_file_block = current_pointer / BYTESPERBLOCK;
  i32 end_file_block = (current_pointer + numb) / BYTESPERBLOCK;

  // check for file expansion to accomodate new data
  i32 file_size = bfsGetSize(inum);

  i32 max_file_block = (file_size - 1) / BYTESPERBLOCK;
  bool expanding_write = false;
  if (end_file_block > max_file_block)
  {
    bfsExtend(inum, end_file_block);
    expanding_write = true;
  }

  // buffer allocation
  i8 total_blocks = end_file_block - start_file_block + 1;
  i8 write_buff[(total_blocks)*BYTESPERBLOCK]; // for entire write operation
  i8 block_buff[BYTESPERBLOCK];                // for individual block operations

  // copy first file block into writebuff
  bfsRead(inum, start_file_block, block_buff);

  // copy last file block into writebuff
  if (total_blocks > 1)
  {
    bfsRead(inum, end_file_block, block_buff);
    memcpy(write_buff + (total_blocks - 1) * BYTESPERBLOCK, block_buff, BYTESPERBLOCK);
  }

  // copy buf into write buff with offset
  memcpy(write_buff + current_pointer % BYTESPERBLOCK, buf, numb);

  // write from write buff to each DBN
  i32 buffer_offset = 0;

  for (i32 i = start_file_block; i <= end_file_block; i++)
  {
    memcpy(block_buff, write_buff + buffer_offset, BYTESPERBLOCK);
    i32 dbn = bfsFbnToDbn(inum, i); // Find a dbn for a given inum and file block
    bioWrite(dbn, block_buff);
    buffer_offset += BYTESPERBLOCK;
  }

  // update file size if needed
  if (expanding_write)
  {
    bfsSetSize(inum, current_pointer + numb);
  }

  fsSeek(fd, numb, SEEK_CUR); // updating file pointer
  return 0;
}
