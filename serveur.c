/* serveur.c
 Serveur HTTP répondant à GET
 */
# include "sys.h"

int
legal_char(char aChar){
  int is_leg = 0;
  if (aChar > 32 && aChar < 122)
    is_leg = 1;
  
  switch(aChar) {
    case '/' :
      is_leg = 0;
    case '\\' :
      is_leg = 0;
    case '?' :
      is_leg = 0;
    case '%' :
      is_leg = 0;
    case '*' :
      is_leg = 0;
    case ':' :
      is_leg = 0;
    case '|' :
      is_leg = 0;
    case '"' :
      is_leg = 0;
    case '<' :
      is_leg = 0;
    case '>' :
      is_leg = 0;
    case '.' :
      is_leg = 0;
  
  }
  return is_leg;
}

int
valid_path(char aPath[]) {
  int i=0, down=0, up=0;
  while (aPath[i]){
    if (aPath[i]=='/') {
      if(i>0 && legal_char(aPath[i-1]))
        down++;
      if ((i==2 || (i>2 && aPath[i-3] == '/')) && aPath[i-1] == '.' && aPath[i-2] == '.')
        up++;
      if ((down-up)<0)
        return down-up;
    }
    if(!aPath[i+1] && legal_char(aPath[i])) //Pas besoin de recalculer down-up car la sortie le fera
      down++;
    i++;
  }
  return down-up;
}

char *
msg_str(int msg) {
  char * strmsg;
  switch (msg) {
  case 200 :
    strmsg = "OK";
    break;
  case 400 :
    strmsg = "Bad Request";
    break;
  case 500 :
    strmsg = "dont know";
    break;
  }
  return strmsg;
}

int
servir(int fd){
  FILE * stream, * file, * config;
  char buffer[1024], path[100], host[100], * asctime_string;
  time_t current_time;
  struct tm * timeinfo;
  int i, cmd, file_sz, msg=0;

  stream = fdopen(fd, "r+");
  if (stream == 0){
    perror("serveur fdopen");
    exit(1);
  }
  setbuf(stream, 0);
  fgets(buffer, sizeof buffer, stream);
  
  
  //j'aurais pu utiliser sscanf mais je souhaitais traiter "en place" la ligne de commande
  asctime_string = strtok(buffer," ");
  for(i=0;i<3;i++) {
    
    if (!*buffer) //Si ligne de commande est trop courte
      msg=400;
    
    switch(i) {

      case 0  :
        if (!strcmp("GET", asctime_string))
          cmd = 1;
        else
          msg = 400;
    
      case 1  :
        // Prend path avant qu'il soit absolu
        strcpy(path, asctime_string);
  
      case 3 :
        //Vérifie bon protocole.
        if (sscanf(asctime_string, "HTTP/%s", asctime_string) == 1)
          if (strcmp("1.0",asctime_string) || strcmp("1.1", asctime_string))
            msg = 400;
    }
    
    asctime_string = strtok (NULL, " ");
    
    if (i==3 && asctime_string != NULL) //Si ligne de commande est trop longue
      msg=400;
  }
  
  while(strlen(fgets(buffer, sizeof buffer, stream)) != 0){
    if(!strcmp(buffer,"\r\n"))
      break;
    if (msg==400)
      break;
      
    if (sscanf(buffer, "Host: %s", host) == 1){
      config = fopen("config","r"); //Prend fichier config, et trouve chemin absolu
      i = 0;
      while(fgets(buffer, sizeof buffer, config) != 0){
        if ((buffer[0] == '#') || (buffer[0] == '\n' ))
          continue;
        if (sscanf(buffer, "ServerName %s", path) && !strcmp(host, path))
            i = 1;
        if (i & sscanf(buffer, "DocumentRoot %s\n", path)){
            fclose(config);
            break;
        }
     }
     
   }
   
   
   
   else{}
      //Autres paramètres non-pris en compte
  }
  

  
  /*Passe au client les valeurs*/
  
  //Process Path, on ne peut pas remonter l'arbre des fichiers au-delà du chemin trouvé dans config.
  
  if (valid_path(path)>0) //est-ce que le chemin est valide
      msg = 400;
  
  if (path[strlen(path)-1]=='/')
    strcat(path, "index.html");
  
  file = fopen(path,"r"); //Prend fichier, ouvre
  if (!file){
    msg = 400;
  }
   
  //OPEN AND LOOK FOR NUMBER.
  fseek(file, 0, SEEK_END);
  file_sz = ftell(file);
  rewind(file);
  
    
  /* Get time format. */
  time(&current_time);
  timeinfo = localtime(&current_time);
  asctime_string = asctime(timeinfo);

  if (!asctime_string)
    msg=500 ;
  
   
  
  fprintf(stream, "HTTP/1.1 %d %s\n", msg, msg_str(msg));
  fprintf(stream, "%s", asctime_string);
  
  fprintf(stream, "Server: Solal's Computer\n");
  fprintf(stream, "Connection: close\n");
  fprintf(stream, "Content-Type: text/html\n");
  fprintf(stream, "Content-Length: %d\n\n", file_sz);
 
  
  while (fgets(buffer, sizeof buffer, file) != 0) {
    fprintf(stream, "%s", buffer);
  }
  
  
  fclose(file);
  fclose(stream);
  
  exit(0);
}

int
main(int ac, char * av[]){
  char * p;

  p = answer("80", servir);
  fprintf(stderr, "repondre a echoue: %s\n", p);
  return 1;
}
