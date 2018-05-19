/* n-answer.c
Repondre en asynchrone aux demandes de connexion sur une socket.
 */
# include "sys.h"

char *
answer(char * port, int (*fun)(int)){
  struct addrinfo * info, * p;
  struct addrinfo indices;
  int fd, t;
  int toclient;
  struct sockaddr fromaddr;
  unsigned int len = sizeof fromaddr;


  memset(&indices, 0, sizeof indices);
  indices.ai_flags = AI_PASSIVE;     // accepter de toutes les machines
  indices.ai_family = AF_INET;       // seulement IPv4
  indices.ai_socktype = SOCK_STREAM; // seulement TCP

  t = getaddrinfo(0, port, &indices, &info);
  if (t != 0)
    return "answer: cannot get info on port";

  // Ouvrir la socket
  for(p = info; p != 0; p = p->ai_next){
    fd = socket(p->ai_family, p->ai_socktype, 0); // fabriquer la socket
    if (fd >= 0)
      break;
  }
  if (p == 0)
    return "answer: pas moyen d'avoir une socket";

  t = 1; // re-utiliser l'adresse
  t = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof t);
  if (t < 0){
    perror("setsockopt");
    fprintf(stderr, "answer: on continue quand meme\n");
  }

  if (bind(fd, p->ai_addr, p->ai_addrlen) < 0){ // donne lui un role
    close(fd);
    return "answer: pas moyen de binder la socket";
  }

  freeaddrinfo(info);

  t = listen(fd, 1);	// on veut des connexions par ici
  if (t < 0){
    close(fd);
    return "answer: pas moyen d'ecouter sur la socket";
  }

  for(;;){
    toclient = accept(fd, &fromaddr, &len);
    if (toclient < 0)
      return "answer: pas moyen d'accepter\n";

    t = fork();			// proc enfant pour le client
    if (t < 0){
      perror("fork");
      continue;
    }

    if (t != 0){ // parent
      close(toclient);
    } else {     // enfant
      close(fd);
      fun(toclient);
      exit(0);
    }
  }
}

# if DEBUG
/*
 Un serveur d'echo pour tester
*/
int
foo(int n){
  char c;
  int t;

  printf("J'ai un client\n");
  while((t = read(n, &c, 1)) > 0)
    if (write(n, &c, 1) != 1){
      perror("write");
      break;
    }
  if (t < 0)
    perror("read");
  
  return 0;
}

int
main(int ac, char * av[]){
  char * p;
  p = answer(av[1], foo);

  perror(p); 
  return 1;
}
# endif /* DEBUG */
