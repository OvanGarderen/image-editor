#include "misc.h"
#include "mode_file.h"

Filevars filevars;

Modespec* init__file(Modespec_el* context) {
  Modespec* spec = init__Modespec(context);
  if(spec) {
    /* initialise vars */
    spec->vars = &filevars;
    sprintf(filevars.filename,"");
    filevars.saved = 1;

    modelog("save","registered");
    logsub("call");
    spec->call = call__file;

    logsub("filename as global filename");
    logsub("saved boolean as global saved");
    register__global_filename(filevars.filename);
    register__global_saved(& filevars.saved);

    logsub("functions");
    register__function("save",comm__save);
    register__function("load",comm__load);
  }
  return spec;
}

void call__file_save(int argnum, char** args) {
  if(argnum == 1){
    if(!strcmp(global.filename,"")){
      set_UImess("no filename given");
    } else {
      save_buffer(global.filename);
      set_UImess("Buffer saved");
    }
  } else {
    int newnum;
    char** newargs;
    char* fn;
    with(newargs, escape_tokenlist(argnum,args,&newnum)) {
      with(fn, strappend(strclone(newargs[1]),".bmp")) {	
	save_buffer(fn);
	set_UImess("Buffer saved");
      }
    }
  }
}

void call__file_load(int argnum, char** args) {
  if(argnum == 1){
    set_UImess("No file to load.");
  } else {
    int newnum;
    char** newargs;
    with(newargs, escape_tokenlist(argnum,args,&newnum)) {
      load_buffer(newargs[1]);
      set_UImess("Loaded %s",newargs[1]);
    };
  }
}

int call__file(Modespec* self, int argnum, char** args) {
  if( argnum > 1 ){
    if(!strcmp(args[1],"save")){
      call__file_save(argnum - 1, &args[1]);
    } else if(!strcmp(args[1],"load")){
      call__file_load(argnum - 1, &args[1]);
    }
  }
  return 0;
}

/*
 * Functions
 */

int comm__save(int argnum, char** args) {
  int ret;
  char* pass_args,* collapsed;
  if(argnum == 1){
    ret = execute_text("file save");
  } else {
    with(collapsed, collapse_tokenlist(argnum-1,&args[1])) {
      with(pass_args, strclone("file save ")) {
	pass_args = strappend(pass_args, collapsed);
	set_UImess("diverting command %s",pass_args);
	ret = execute_text(pass_args);
      }
    }
  }
  pull_cur_mode();
  return ret;
}

int comm__load(int argnum, char** args){
  int ret;
  char* pass_args,* collapsed;
  if(argnum == 1){
    ret = execute_text("file save");
  } else {
    with(collapsed, collapse_tokenlist(argnum-1,&args[1])) {
      with(pass_args, strclone("file load ")) {
	pass_args = strappend(pass_args, collapsed);
	set_UImess("diverting command %s",pass_args);
	ret = execute_text(pass_args);
      }
    }
  }
  pull_cur_mode();
  return ret;
}
