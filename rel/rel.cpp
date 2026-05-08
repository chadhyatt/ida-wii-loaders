/*
*  IDA Nintendo GameCube rel Loader Module
*  (C) Copyright 2013 Stephen Simpson
*
*/

#include "rel.h"
#include "rel_track.h"



/*-----------------------------------------------------------------
*
*   Check if input file can be a rel file. The supposed header
*   is checked for sanity. If so return and fill in the formatname
*   otherwise return 0
*
*/

int idaapi accept_file(qstring *fileformatname, qstring *processor, linput_t *li, const char *filename)
{
  rel_track test_valid(li);

  // Check if valid
  if (!test_valid.is_good())
    return 0;

  // file has passed all sanity checks and might be a rel
  *fileformatname = "Nintendo REL";
  *processor = "PPC";
  return(ACCEPT_FIRST | 0xD07);
}



/*-----------------------------------------------------------------
*
*   File was recognised as rel and user has selected it.
*   Now load it into the database
*
*/

void idaapi load_file(linput_t *fp, ushort neflag, const char * /*fileformatname*/)
{
  // Hello here I am
  msg("---------------------------------------\n");
  msg("Nintendo REL Loader Plugin 0.1\n");
  msg("---------------------------------------\n");

  // we need PowerPC support to do anything with rels
  if (PH.id != PLFM_PPC)
    set_processor_type("PPC", SETPROC_LOADER);

  set_compiler_id(COMP_GNU);

  inf_set_app_bitness(32);
  inf_set_be(true);

  rel_track track(fp);
  inf_set_start_ea(START);
  inf_set_start_ip(START);

  // map selector 1 to 0
  set_selector(1, 0);



  track.apply_patches();
}

/*-----------------------------------------------------------------
*
*   Loader Module Descriptor Blocks
*
*/

loader_t LDSC =
{
  IDP_INTERFACE_VERSION,
  0,
  accept_file,
  load_file,
  nullptr,
  nullptr,
  nullptr,
};
