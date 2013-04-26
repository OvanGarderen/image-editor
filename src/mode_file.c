#include "misc.h"
#include "mode_file.h"
#include "inputhandler.h"

Filevars filevars;

Modespec* init__file(Modespec_el* context) {
  Modespec* spec = init__Modespec(context);
  if(spec) {
    /* initialise vars */
    spec->vars = &filevars;
    sprintf(filevars.filename,"%s","");
    filevars.saved = 1;

    modelog("save","registered");
    logsub("call");
    spec->call = call__file;
    spec->activate = activate__file;
    
    logsub("filename as global filename");
    logsub("saved boolean as global saved");
    register__global_filename(&global,filevars.filename);
    register__global_saved(&global,&filevars.saved);

    logsub("functions");
    register__function(&global,"save",comm__save);
    register__function(&global,"load",comm__load);
  }
  return spec;
}

void activate__file(Modespec* self, char* arg) {
  if(!strcmp(arg,"save")) {
    set_UImmode(&global,"bmp");
    Bufferspec spec = (Bufferspec)
      {CBtype_command,"saving to","save",NULL};
    start_Inputbuffer(spec);
  } else if(!strcmp(arg,"load")) {
    Bufferspec spec = (Bufferspec)
      {CBtype_command,"loading from","load",NULL};
    start_Inputbuffer(spec);
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

void call__file_save(int argnum, char** args) {
  if(argnum == 1){
    if(!strcmp(global.filename,"")){
      set_UImess(&global,"no filename given");
    } else {
      save_buffer(&global,global.filename);
      set_UImess(&global,"Buffer saved");
    }
  } else {
    int newnum;
    char** newargs;
    char* fn;
    with(newargs, escape_tokenlist(argnum,args,&newnum)) {
      with(fn, strappend(strclone(newargs[1]),".bmp")) {	
	save_buffer(&global,fn);
	set_filename(&global,fn);
	*global.saved = 1;
	set_UImess(&global,"Buffer saved");
      }
    }
  }
}

void call__file_load(int argnum, char** args) {
  if(argnum == 1){
    set_UImess(&global,"No file to load.");
  } else {
    int newnum;
    char** newargs;
    with(newargs, escape_tokenlist(argnum,args,&newnum)) {
      load_buffer(&global,newargs[1]);
      set_filename(&global,newargs[1]);
      *global.saved = 1;
      set_UImess(&global,"Loaded %s",newargs[1]);
    };
  }
}

/*
 * Functions
 */

int comm__save(int argnum, char** args) {
  int ret;
  char* pass_args,* collapsed;
  if(argnum == 1){
    ret = execute_text(&global,"file save");
  } else {
    with(collapsed, collapse_tokenlist(argnum-1,&args[1])) {
      with(pass_args, strclone("file save ")) {
	pass_args = strappend(pass_args, collapsed);
	set_UImess(&global,"diverting command %s",pass_args);
	ret = execute_text(&global,pass_args);
      }
    }
  }
  pull_cur_mode(&global);
  return ret;
}

int comm__load(int argnum, char** args){
  int ret;
  char* pass_args,* collapsed;
  if(argnum == 1){
    ret = execute_text(&global,"file save");
  } else {
    with(collapsed, collapse_tokenlist(argnum-1,&args[1])) {
      with(pass_args, strclone("file load ")) {
	pass_args = strappend(pass_args, collapsed);
	set_UImess(&global,"diverting command %s",pass_args);
	ret = execute_text(&global,pass_args);
      }
    }
  }
  pull_cur_mode(&global);
  return ret;
}
