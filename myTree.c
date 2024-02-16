#include "myTree.h"

//COMPILAZIONE: gcc myTree.c -o myTree -lm


//COLORAZIONE

void blue()     { printf("\033[1;34m"); }
void green()    { printf("\033[1;32m"); }
void purple()   { printf("\033[1;35m"); }
void red()      { printf("\033[1;31m"); }
void cyan()     { printf("\033[1;36m"); }
void reset()    { printf("\x1b[0m");    }


//FUNZIONI AUSILIARIE

void info_options(tree_options options, struct stat file_info) {

    /* Gestione opzioni: --inodes, -p, -u, -g, -s, -D */

    if (options.inodes || options.permissions || options.size || options.username || options.groupname || options.date) {
        
        printf("[");
        if (options.inodes) {
            display_inodes(file_info);
        }
        if (options.permissions) {
            if (options.inodes) printf(" ");
            display_perms(file_info);
        }
        if (options.username){
            if (options.inodes || options.permissions) printf(" ");
            display_username(file_info);
        }
        if (options.groupname){
            if (options.inodes || options.permissions) printf(" ");
            display_groupname(file_info);
        }
        if (options.size) {
            display_size(file_info);
        }
        if (options.date) {
            if (options.inodes || options.permissions || options.size) printf(" ");
            display_date(file_info);
        }

        printf("]  ");
    }
}

bool is_image(char * ext) {

    /* Funzione per rilevare se un file è un' immagine in base alla sua estensione */

    int size;
    const char * img_exts[] = {".avi", ".bmp", ".cgm", ".dl", ".emf", ".fli", ".flc", ".flv", ".gif", ".gl",
                          ".jpeg", ".jpg", ".m2v", ".mkv", ".m4v", ".mov", ".mp4", ".mp4v", ".mpg", ".mpeg",
                          ".mjpeg", ".mjpg", ".mng", ".nuv", ".ogm", ".pbm", ".pcx", ".pgm", ".png", ".ppm",
                          ".qt", ".rm", ".rmvb", ".svg", ".svgz", ".tga", ".tif", ".tiff", ".vob", ".webm",
                          ".webp", ".wmv", ".xbm", ".xcf", ".xpm", ".xwd", ".yuv"};

    size = sizeof(img_exts)/sizeof(img_exts[0]);

    for (int i=0; i < size; i++) {
        if (strcmp(ext, img_exts[i]) == 0) return true;
    }
    return false;                   
}


bool is_compressed(char * ext) {

    /* Funzione per rilevare se un file è compresso in base alla sua estensione */

    int size;
    const char * compressed_exts[] = {".tar", ".tgz", ".arc", ".arj", ".taz", ".lha", ".lz4", ".lzh", ".lzma", ".tlz",
                                ".txz", ".tzo", ".t7z", ".zip", ".z", ".dz", ".gz", ".lrz", ".lz", ".lzo", ".xz",
                                ".zst", ".tzst", ".bz2", ".bz", ".tbz", ".tbz2", ".tz", ".deb", ".rpm", ".jar", ".war",
                                ".ear", ".sar", ".rar", ".alz", ".ace", ".zoo", ".cpio", ".7z", ".rz", ".cab", ".wim",
                                ".swm", ".dwm", ".esd"};



    size = sizeof(compressed_exts)/sizeof(compressed_exts[0]);

    for (int i=0; i < size; i++) {
        if (strcmp(ext, compressed_exts[i]) == 0) return true;
    }
    return false;
}

time_t get_mtime(char * filename, char * dir_path) {

    /* Funzione che ritorna l'mtime del file in input */

    struct stat file_stat;
    char file_path[PATH_MAX];
    snprintf(file_path, PATH_MAX, "%s/%s", dir_path, filename);
    lstat(file_path, &file_stat);
    return file_stat.st_mtime;
}


void no_punctuation(char* str) {

    /* Funzione per rimuovere la punteggiatura nella stringa passata in input */
    char* p = str;
    while (*p) {
        if (ispunct(*p)) {
            memmove(p, p + 1, strlen(p));
        } else {
            ++p;
        }
    }
}


void display_help() {

    /* Funzione per l'opzione --help -> Stampa l'help di tree */

    printf("\n usage: ./myTree [-adfpsugDrt] [-L level] [--inodes] [--dirsfirst] [--help] [directory ...]\n");

    printf("\n> Description: \n\n");

    printf("\tmyTree is a recursive directory listing program that produces a depth indented listing of files.\n");
    printf("\tWith no arguments, myTree lists the files in the current directory.\n");
    printf("\tWhen directory arguments are given, myTree lists all the files and/or directories found in the given directories each in turn.\n");
    printf("\tUpon completion of listing all files/directories found, myTree returns the total number of files and/or directories listed.\n");

    printf("\n> Listing options: \n\n");

    printf("\t-a: \t\tAll files are printed. By default tree does not print hidden files (those beginning with a dot '.').\n");
    printf("\t-d: \t\tList directories only.\n");
    printf("\t-f: \t\tPrints the full path prefix for each file.\n");
    printf("\t-p: \t\tPrint the file type and permissions for each file.\n");
    printf("\t-s: \t\tPrint the size of each file in bytes along with the name.\n");
    printf("\t-u: \t\tPrint the username, or UID # if no username is available, of the file.\n");
    printf("\t-g: \t\tPrint the group name, or GID # if no group name is available, of the file.\n");
    printf("\t-D: \t\tPrint the date of the last modification time for the file listed.\n");
    printf("\t-r: \t\tSort the output in reverse alphabetic order.\n");
    printf("\t-t: \t\tSort the output by last modification time instead of alphabetically.\n");
    printf("\t-L level: \tMax display depth of the directory tree.\n");
    printf("\t--inodes: \tPrints the inode number of the file or directory.\n");
    printf("\t--dirsfirst: \tList directories before files.\n");
    printf("\t--help: \tOutputs a verbose usage listing. \n\n");
}

void display_inodes(struct stat file_info) {

    /* Funzione per l'opzione --inodes -> Si occupa di stampare l'inode del file */

    long int inode = file_info.st_ino;

    int len_inode  = inode == 0 ? 1: (int)log10(inode)+1; //len di size (numero di digit)
    for(int i=0; i < (7-len_inode); i++) printf(" ");  //print spazio

    printf("%ld", inode);

}

void display_perms(struct stat file_info) {

    /* Funzione per l'opzione -p -> Si occupa di stampare i permessi del file */

    //symbolic links
    if (S_ISLNK(file_info.st_mode)) printf("l");
    //directory
    if (!(S_ISLNK(file_info.st_mode))) printf((S_ISDIR(file_info.st_mode))  ? "d": "-");

    //user
    printf((file_info.st_mode & S_IRUSR)  ? "r": "-");
    printf((file_info.st_mode & S_IWUSR)  ? "w": "-"); 
    if (file_info.st_mode & S_ISUID)    {printf((file_info.st_mode & S_IXUSR) ? "s": "S");} //setuid
    if (!(file_info.st_mode & S_ISUID)) {printf((file_info.st_mode & S_IXUSR)  ? "x": "-");}

    //group
    printf((file_info.st_mode & S_IRGRP)  ? "r": "-");
    printf((file_info.st_mode & S_IWGRP)  ? "w": "-");
    if (file_info.st_mode & S_ISGID)    {printf((file_info.st_mode & S_IXGRP) ? "s": "S");} //setgid
    if (!(file_info.st_mode & S_ISGID)) {printf((file_info.st_mode & S_IXGRP)  ? "x": "-");}

    //other
    printf((file_info.st_mode & S_IROTH)  ? "r": "-");
    printf((file_info.st_mode & S_IWOTH)  ? "w": "-");
    if (file_info.st_mode & S_ISVTX)    {printf((file_info.st_mode & S_IXOTH) ? "t": "T");} //stickybit
    if (!(file_info.st_mode & S_ISVTX)) {printf((file_info.st_mode & S_IXOTH)  ? "x": "-");}

}

void display_size(struct stat file_info) {

    /* Funzione per l'opzione -s -> Si occupa di stampare la dimensione del file in bytes */

    long int size = file_info.st_size;

    //spazio nelle [ ], 
    //ad esempio se la size == 4096 allora su terminale risulterà [       4096]

    int len_size  = size == 0 ? 1: (int)log10(size)+1; //len di size (numero di digit)
    for(int i=0; i < (11-len_size); i++) printf(" ");  //print spazio

    printf("%ld", size);
}

void display_username(struct stat file_info) {

    /* Funzione per l'opzione -u -> Si occupa di stampare l'username o il #UID se non disponibile */

    struct passwd * pwd;

    //Username
    if ((pwd = getpwuid(file_info.st_uid)) != NULL) printf("%s ", pwd -> pw_name);
    //#UID
    else printf("%d ", file_info.st_uid);
}

void display_groupname(struct stat file_info) {

    /* Funzione per l'opzione -g -> Si occupa di stampare il groupname o il #GID se non disponibile */

    struct group * grp;

    //Groupname
    if ((grp = getgrgid(file_info.st_gid)) != NULL) printf("%s ", grp -> gr_name);
    //#GID
    else printf("%d ", file_info.st_gid);
}

void display_date(struct stat file_info) {

    /* Funzione per l'opzione -D -> Si occupa di stampare l'ultima data di modifica del file
        - Formati di stampa:
            Mese-Giorno-Ora  (se l'ultima modifica è avvenuta nell'anno corrente)
            Mese-Giorno-Anno (se l'ultima modifica è avvenuta in un anno diverso dal corrente)
    */

    struct tm * tm;
    struct tm * file_tm;
    char date_string[100];
    int curr_year;
    int file_year;

    //Anno corrente
    time_t t = time(NULL);
    tm = localtime(&t);
    curr_year = tm->tm_year+1900;

    //Anno di ultima modifica del file
    file_tm = localtime(&file_info.st_mtime);
    file_year = file_tm->tm_year+1900;

    //Se il file è stato modificato l'ultima volta nell'anno corrente -> printa ora di ultima modifica
    if (file_year == curr_year) {
        strftime(date_string, sizeof(date_string), "%b %d %H:%M", file_tm);
    }
    //Altrimenti l'anno dell'ultima modifica
    else {
        strftime(date_string, sizeof(date_string), "%b %d  %Y", file_tm);
    }
    printf("%s", date_string);
}


//ORDINAMENTO

int alpha_order(const void* a, const void* b) {

    /* Funzione per l'ordinamento alfabetico di default */

    char* str_a = strdup(*(const char**)a);
    char* str_b = strdup(*(const char**)b);

    // Rimossa la punteggiatura
    no_punctuation(str_a);
    no_punctuation(str_b);

    return strcasecmp(str_a, str_b);
}

void merge_sort(char ** arr, int l, int r, char * dir_path)
{
    
    /* Merge sort modificato per l'opzione -t -> Ordina l'albero in base all'ultima data di modifica */

    if (l < r) {
        int m = (l+r)/2;
        merge_sort(arr, l, m, dir_path);
        merge_sort(arr, m + 1, r, dir_path);
        merge(arr, l, m, r, dir_path);
    }
}

void merge(char ** arr, int l, int m, int r, char * dir_path)
{
    int i, j, k;
    int n = r-l+1;

    char ** temp = (char **) malloc(n*sizeof(char*));
 
    i = l; 
    j = m+1; 
    k = 0; 
    while (i <= m && j <= r) {

        time_t t1 = get_mtime(arr[i], dir_path);
        time_t t2 = get_mtime(arr[j], dir_path);
        if (t1 < t2) {
            temp[k] = arr[i];
            i++;
        }

        else if (t1 > t2) {
            temp[k] = arr[j];
            j++;
        }

        //Se la t1 == t2 -> ordinamento alfabetico
        else {
            char * s1 = strdup(arr[i]);
            char * s2 = strdup(arr[j]);

            no_punctuation(s1);
            no_punctuation(s2);

            if (strcasecmp(s1, s2) <= 0) {
                temp[k] = arr[i];
                i++;
            }
        }
        k++;
    }

    while (i <= m) {
        temp[k] = arr[i];
        i++;
        k++;
    }
 
    while (j <= r) {
        temp[k] = arr[j];
        j++;
        k++;
    }

    for (int i=l, k=0; i<=r; i++, k++){
        arr[i] = temp[k]; 
    }

    free(temp);
}


void reverse(char ** files, int n) {

    /* Funzione per l'opzione -r -> Inverte l'ordine di stampa */

    for (int i=0, j=n-1; i<j; i++, j--) {
        char * temp = files[i];
        files[i] = files[j];
        files[j] = temp;
    }
}



void dirs_order(char ** files, int n, char * dir_path) {

    /* Funzione per l'opzione --dirsfirst -> Permette di stampare prima le directory dei file */

    char file_path[PATH_MAX];
    struct stat file_stat;
    int dir_index  = 0;
    int file_index = 0;

    //Array per le directory
    char ** temp_dirs  = (char **) malloc(n * sizeof(char*));
    if (temp_dirs == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    //Array per i file
    char ** temp_files = (char **) malloc(n * sizeof(char*));
    if (temp_files == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int i=0; i < n; i++) {
        snprintf(file_path, PATH_MAX, "%s/%s", dir_path, files[i]);
        lstat(file_path, &file_stat);
        
        //Se è una DIRECTORY -> Inserito in temp_dirs
        if (S_ISDIR(file_stat.st_mode)) {
            temp_dirs[dir_index] = files[i];
            dir_index++;
            continue;
        }

        //Se è un FILE -> Inserito su temp_files
        else  {
            temp_files[file_index] = files[i];
            file_index++;
            continue;
        }
    }

    //Ordinamento
    for (int i=0; i < dir_index; i++) {
        files[i] = temp_dirs[i];
    }

    for (int i=dir_index, j=0; j < file_index; i++, j++) {
        files[i] = temp_files[j];
    }

    free(temp_dirs);
    free(temp_files);

}

static int tree(char * dir_path, int depth, int * file_count, 
                    int * dir_count, tree_options options, int level, const char * indent) {

    /* Costruisce l'albero a partire dalla directory con path <dir_path> gestendo eventuali opzioni indicate */
    
    DIR * dir;
    struct dirent * d_entry;
    struct stat file_info;
    char ** files = NULL;
    int n_files = 0;

    dir = opendir(dir_path);

    //Se non è possibile aprire la directory
    if (! dir) {
        reset();
        printf("  [error opening dir]");
        return 0;
    }

    //Calcolo numero di files != "." e ".." 
    while ((d_entry = readdir(dir)) != NULL) {

        if (strcmp(d_entry->d_name, ".") != 0 && strcmp(d_entry->d_name, "..") != 0) {
            n_files++;
        }
    }

    //Allocazione dinamica -> necessaria per ordinamento
    files = (char **) malloc(n_files * sizeof(char *));
    if (files == NULL) {
        perror("main: tree: malloc");
        closedir(dir);
        exit(EXIT_FAILURE);
    }

    rewinddir(dir);

    //Assegnamento di valori all'array
    int i = 0;
    char path[PATH_MAX];
    while ((d_entry = readdir(dir)) != NULL) {
        
        //Path corrente
        snprintf(path, PATH_MAX, "%s/%s", dir_path, d_entry->d_name);

        if (lstat(path, &file_info) == -1) {
            perror("main: tree: lstat");
            return 1;
        }
        
        //Se il file è != "." e ".."
        if (strcmp(d_entry-> d_name, ".") != 0 && strcmp(d_entry->d_name, "..") != 0) {

            // -a option -> show hidden files 
            if (!options.hidden_files && (d_entry->d_name)[0] == '.') {
                n_files--;
                continue;
            }

            //-d option -> show only dirs
            if (options.only_dirs) {
                struct stat link_info;

                //se non è una dir o un link -> skip
                if (!S_ISDIR(file_info.st_mode) && !S_ISLNK(file_info.st_mode)) {
                    n_files--;
                    continue;
                }

                //se è un link ma non punta una dir -> skip
                else if (S_ISLNK(file_info.st_mode)) {
                    
                    if (stat(path, &link_info) != -1) {
                        if (!S_ISDIR(link_info.st_mode)) {
                            n_files--;
                            continue;
                        }
                    }

                    else {
                        n_files--;
                        continue;
                    }
                }   
            }

            files[i] = strdup(d_entry-> d_name);
            i++;
        }
    }

    //Ordinamento alfabetico (default)
    qsort(files, n_files, sizeof(char *), alpha_order);

    //-t option -> ordina in base all'ultima data di modifica
    if (options.last_modify) {
        merge_sort(files, 0, n_files-1, dir_path);
    }

    //-r option -> ordina al contrario
    if (options.reverse_order) {
        reverse(files, n_files);
    }

    //--dirsfirst -> prima le directory e poi i file
    if (options.dirs_first) {
        dirs_order(files, n_files, dir_path);
    }

    //Iterazione su array (ordinato)
    for (int j=0; j < n_files; j++) {

        //Path corrente
        char child_path[PATH_MAX];
        snprintf (child_path, PATH_MAX, "%s/%s", dir_path, files[j]);


        //lstat permette di riconoscere i link symbolici, stat segue direttamente il link non riconoscendolo (usato più tardi)
        if (lstat(child_path, &file_info) == -1) {
            perror("main: tree: lstat");
            return 1;
        }


        //Gestione indentazione

        //Prima si printa l'indentazione che viene aggiornata ogni volta che si va in ricorsione (parametro "indent")
        //Successivamente, 
        //Se il file corrente è l'ultimo della directory attuale -> └──
        //Altrimenti -> ├──
        reset();
        printf("\n");
        char * last_item = (j == n_files-1) ? "└── ": "├── ";
        printf("%s%s", indent, last_item);

        //-f option -> show full path
        if (options.full_path) {
            files[j] = strdup(child_path);
        }

        //opzioni che aggiungono dei parametri davanti al nome dell'entry della directory analizzata
        //devono essere stampati in questo ordine: --inodes, -p, -u, -g, -s, -D
        //La funzione info_options contiene tutto ciò che riguarda la gestione di queste opzioni
        info_options(options, file_info);

        // Se è una DIRECTORY 
        if (S_ISDIR(file_info.st_mode)) {

            blue();
            printf("%s", files[j]);
            (*dir_count)++;


            //Richiama ricorsivamente tree con il nuovo path calcolato
            //se l'opzione -L level è presente entra in ricorsione solo se il massimo livello non è stato raggiunto
            if (options.level_depth && ((depth+1) == level)){
                continue;
            }

            //Altrimenti -> RICORSIONE
            else { 
                
                //Gestione indentazione (per quanto riguarda il livello)
                char subdir_indent[PATH_MAX];
                snprintf(subdir_indent, PATH_MAX, "%s%s", indent, (j == n_files-1) ? "    ": "│   ");
                //Chiamata ricorsiva
                tree(child_path, depth+1, file_count, dir_count, options, level, subdir_indent);
            }
        }

        //Se è un LINK
        else if (S_ISLNK(file_info.st_mode)) {

            char linked_file[PATH_MAX];

            //Lettura del valore puntato dal link
            ssize_t bytes_read = readlink(child_path, linked_file, sizeof(linked_file) - 1);
            //Check error
            if (bytes_read == -1) {
                perror("main: tree: readlink");
                return 1;
            }

            linked_file[bytes_read] = '\0';
            
            cyan();
            printf("%s", files[j]);
            reset();
            printf(" -> ");

            //stat fa riferimento al file puntato dal link
            stat(child_path, &file_info);

            //Se il file puntato è una DIRECTORY
            if (S_ISDIR(file_info.st_mode)) {
                
                blue();
                printf("%s", linked_file);
                (*dir_count)++;
            }

            //Altrimenti
            else {

                if (S_ISREG(file_info.st_mode)) {

                    //Calcolo estensione del file
                    char * ext = strrchr(linked_file, '.');

                    // Se è eseguibile allora colora di verde
                    if (file_info.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                        green();
                    }
                    
                    //Se ha un'estensione
                    else if (ext != NULL) {
                        
                        //Viola se immagine
                        if (is_image(ext)) {
                            purple();
                        }

                        //Rosso se compresso
                        else if (is_compressed(ext)) {
                            red();
                        }
                    } 

                    //Altrimenti default
                    else {
                        reset();
                    }
                }
                
                printf("%s", linked_file);
                (*file_count)++;
            }
        }

        // Se è un FILE
        else {
             
            //Calcolo estensione del file
            char * ext = strrchr(files[j], '.');

            // Se è eseguibile allora colora di verde
            if (file_info.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                green();
            }
            
            //Se ha un'estensione
            else if (ext != NULL) {
                
                //Viola se immagine
                if (is_image(ext)) {
                    purple();
                }

                //Rosso se compresso
                else if (is_compressed(ext)) {
                    red();
                }
            } 

            //Altrimenti bianco
            else {
                reset();
            }
            
            printf("%s", files[j]);
            (*file_count)++;

        }
    }

    //Chiusura directory e deallocazione
    free(files);
    closedir(dir);
}



int main(int argc, char **argv){
    
    tree_options options;
    struct stat file_info;
    char * dir_name;
    int opt;
    int level = -1; //-1 -> non impostato
    int file_count = 0;
    int dir_count = 0;

    //Raccoglimento opzioni indicate dall'utente
    while((opt = getopt_long(argc, argv, "hadfipsugDL:rtF", long_options, NULL)) != -1){
        switch(opt){

            case 'a':
                options.hidden_files = true;
                break;
            
            case 'd':
                options.only_dirs = true;
                break;

            case 'f':
                options.full_path = true;
                break;

            case 'i':
                options.inodes = true;
                break;
            
            case 'p':
                options.permissions = true;
                break;
            
            case 's':
                options.size = true;
                break;
            
            case 'u':
                options.username = true;
                break;
            
            case 'g':
                options.groupname = true;
                break;
            
            case 'D':
                options.date = true;
                break;
            
            case 'L':

                level = atoi(optarg);

                if (level <= 0) {
                    printf("myTree: Invalid level, must be greater than 0.\n");
                    exit(EXIT_FAILURE);
                }

                options.level_depth = true;
                break;

            case 'r':
                options.reverse_order = true;
                break;

            case 't':
                options.last_modify = true;
                break;
            
            case 'F':
                options.dirs_first = true;
                break;

            case 'h':
                display_help();
                exit(EXIT_SUCCESS);

            default:
                printf("usage: ./myTree [-adfpsugDrt] [-L level] [--inodes] [--dirsfirst] [--help] [directory ...]\n");
                exit(EXIT_FAILURE);
        }
    }

    //dir_name -> directory indicata dall'utente o se non indicata directory corrente (".")
    dir_name = optind < argc ? argv[optind] : ".";
    
    //Check filename
    DIR * root = opendir(dir_name);

    //Se non è possibile aprire la directory
    if (! root) {
        reset();
        printf("%s  [error opening dir]", dir_name);
        printf("\n\n%d directories\n", dir_count);
        exit(EXIT_FAILURE);
    }


    //stat del file corrente
    if (stat(dir_name, &file_info) == -1) {
        perror("main: stat");
        exit(EXIT_FAILURE);
    }

    //options 
    info_options(options, file_info);
    

    //print e chiamate a funzione
    blue();
    printf("%s", dir_name);
    tree(dir_name, 0, &file_count, &dir_count, options, level, "");
    reset();

    char * dir  = (dir_count == 1)  ? "directory":"directories";
    char * file = (file_count == 1) ? "file":"files";

    if (options.only_dirs) {
        printf("\n\n%d %s\n", dir_count, dir);
    }
    else {
        printf("\n\n%d %s, %d %s\n", dir_count, dir, file_count, file);
    }
}
