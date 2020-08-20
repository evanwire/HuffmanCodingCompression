#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <unistd.h>
#include <dirent.h>

typedef struct Node{
    int occurences;
    char* data;
    struct Node* left;
    struct Node* right;
}Node;

typedef struct MinHeap{
    int size;
    int capacity;
    Node** arr;
}MinHeap;

typedef struct Token{
    char* data;
    int occurences;
}Token;

int fileFound = 0;

int codebookWriter;
int numTokens = 0;
Token** tokens;

int space_counter = 0;
int tab_counter = 0;
int newline_counter = 0;



char space[] = "ssss";
char tab[] = "tttt";
char newline[] = "nnnn";

int change_space = 0;
int change_tab = 0;
int change_newline = 0;

void swap(Token** tokens, int a, int b)  
{  
    Token* temp = tokens[a];
    tokens[a] = tokens[b];
    tokens[b] = temp;
}  
  
int partition (Token** tokens, int low, int high)  
{  
    int pivot = tokens[high]->occurences; 
    int i = (low - 1);   
    int j;
    for (j = low; j <= high - 1; j++)  
    {  
  
        if (tokens[j]->occurences < pivot)  
        {  
            i++; 
            swap(tokens, i, j);  
        }  
    }  
    swap(tokens, i + 1, high);  
    return (i + 1);  
}  

int expand_buffer(char* buff, int size){
    char temp[size];
    strcpy(temp, buff);
    size *= 2;
    char* temp_ptr = realloc(buff, size);
    if(temp_ptr == NULL){
        exit(1);
    }         
    buff = temp_ptr;
    strcpy(buff, temp);
    return size;
}




void quickSort(Token** tokens, int low, int high)  
{  
    if (low < high)  
    {  

        int pi = partition(tokens, low, high);  
  

        quickSort(tokens, low, pi - 1);  
        quickSort(tokens, pi + 1, high);  
    }  
} 

void printNewTree(Node* root){
    if(root == NULL){
        return;
    }
    if(strcmp(root->data, "") == 0){
        printf("EMPTY\n");
    }else{
        printf("%s\n", root->data);
    }
    printNewTree(root->left);
    printNewTree(root->right);
}

struct Node* newNode(char* data, unsigned occurences){ 
    struct Node* temp = ( Node*)malloc(sizeof( Node)); 
  
    temp->left = temp->right = NULL; 
    temp->data = data; 
    temp->occurences = occurences; 
  
    return temp; 
} 

Node* insertNode(Node* root, char* data, char* binary, int binarySize){
    Node* ptr = root;
    int i;

    
    for(i = 0; i < binarySize; i++){
        if(i == binarySize - 1){
            if(binary[i] == '0'){
                ptr->left = newNode(data, 0);
                break;
            }else{
                ptr->right = newNode(data, 0);
                break;
            }
        }else{
            if(binary[i] == '0'){
                if(ptr->left == NULL){
                    ptr->left = newNode("", 0);
                }
                ptr = ptr->left;
            }else{
                if(ptr->right == NULL){
                    ptr->right = newNode("", 0);
                }
                ptr = ptr->right;
            }
        }



    }

    
    
    return root;

}



Node* recreateHuffmanTree(int fd, Node* root){
    int i;
    char* read_in_buff = malloc(1000 * sizeof(char));
    int num_bytes;


    int binary_size = 100;
    char binary[binary_size];
    int binary_idx = 0;

    int token_size = 100;
    char token[token_size];
    int token_idx = 0;

    char header[14];

    
    int readingBinary = 1;
    num_bytes = read(fd, header, 15);
    if(num_bytes < 0){
        exit(1);
    }

    for(i = 0; i < 14; i++){
        if(i < 4){
            space[i] = header[i];
        }else if(i >4 && i <9){
            tab[i - 5] = header[i];
        }else if(i > 9 && i < 14){
            newline[i - 10] = header[i];
        }
    }


    char* tempBinary;
    char* tempToken;

    while(1){
        num_bytes = read(fd, read_in_buff, 1000);
        if(num_bytes < 0){
            exit(1);
        }

        for(i = 0; i < num_bytes; i++){
            if(read_in_buff[i] == '\t'){
                if(strcmp(binary, "") != 0){
                    binary[binary_idx] = '\0';

                    tempBinary = malloc(binary_idx * sizeof(char));
                    strcpy(tempBinary, binary);
                }
                readingBinary = 0;
            }else if(read_in_buff[i] == '\n'){
                readingBinary = 1;
                if(strcmp(token, "") != 0){
                    token[token_idx] = '\0';

                    tempToken = malloc(token_idx * sizeof(char));
                    strcpy(tempToken, token);

                    root = insertNode(root, tempToken, tempBinary, binary_idx);
                    memset(token, 0, sizeof(token));
                    token_idx = 0;
                    memset(binary, 0, sizeof(binary));
                    binary_idx = 0;
                    
                }
            }else if(readingBinary == 1){
                binary[binary_idx] = read_in_buff[i];
                binary_idx++;
                if(binary_idx == binary_size){
                    binary_size = expand_buffer(binary, binary_size);
                }
            }else if(readingBinary == 0){
                token[token_idx] = read_in_buff[i];
                token_idx++;
                if(token_idx == token_size){
                    token_size = expand_buffer(token, token_size);
                }
            }

                
            

        }
        
        break;
    }
    free(read_in_buff);
    close(fd);
    return root;
    
}


  
  
struct MinHeap* createMinHeap(unsigned capacity) { 
  
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap)); 
    minHeap->size = 0; 
    minHeap->capacity = capacity; 
    minHeap->arr = (struct Node**)malloc(minHeap->capacity * sizeof(struct Node*)); 
    return minHeap; 
} 
  

void swapNode(struct Node** a, struct Node** b) { 
    struct Node* t = *a; 
    *a = *b; 
    *b = t; 
} 

int fixDelimeters(Token** tokens, int numTokens){
    int i;
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(change_space == 1){
        while(change_space == 1){
            //printf("\n\ntest\n\n");
            for(i = 0; i < 4; i++){
                int n = rand() % (int) (sizeof charset - 1);
                space[i] = charset[n];
            }
            
            change_space = tokenSearch(space, tokens, numTokens);
        }
    }

    if(change_tab == 1){
        while(change_tab == 1){
            for(i = 0; i < 4; i++){
                int n = rand() % (int) (sizeof charset - 1);
                tab[i] = charset[n];
            }
            change_tab = tokenSearch(tab, tokens, numTokens);
        }
    }
    if(change_newline == 1){
        while(change_newline == 1){
            for(i = 0; i < 4; i++){
                int n = rand() % (int) (sizeof charset - 1);
                newline[i] = charset[n];
            }
            change_newline = tokenSearch(newline, tokens, numTokens);
        }
    }
    


    for(i = 0; i < space_counter; i++){
        numTokens =insertArr(tokens, space, numTokens);
    }
    for(i = 0; i < tab_counter; i++){
        numTokens =insertArr(tokens, tab, numTokens);
    }
    for(i = 0; i < newline_counter; i++){
        numTokens =insertArr(tokens, newline, numTokens);
    }

    return numTokens;

}

void checkFDError(int size){
    if(size == -1){
        perror("Error writing to the Huffman Codebook.\n Response: FATAL ERROR\n");
        exit(1);
    }else if(size == 0){
        perror("Error: the end of the file was reached. Some of your data was lost.\nResponse: Continue operation, with some data lost.\n");
        return;
    }
}
  
void minHeapify(struct MinHeap* minHeap, int idx) { 
    int smallest = idx; 
    int left = 2 * idx + 1; 
    int right = 2 * idx + 2; 
  
    if (left < minHeap->size && minHeap->arr[left]->occurences < minHeap->arr[smallest]->occurences){
        smallest = left; 
    }
  
    if (right < minHeap->size && minHeap->arr[right]->occurences < minHeap->arr[smallest]->occurences) {
        smallest = right; 
    }
  
    if (smallest != idx) { 
        swapNode(&minHeap->arr[smallest], &minHeap->arr[idx]); 
        minHeapify(minHeap, smallest); 
    } 
} 
  
int isSizeOne(struct MinHeap* minHeap) 
{ 
    return (minHeap->size == 1); 
} 
  

struct Node* extractMin(struct MinHeap* minHeap) { 
    struct Node* temp = minHeap->arr[0]; 
    minHeap->arr[0] = minHeap->arr[minHeap->size - 1]; 
  
    --minHeap->size; 
    minHeapify(minHeap, 0); 
  
    return temp; 
} 
  

void insertMinHeap(struct MinHeap* minHeap, struct Node* Node)  {
  
    ++minHeap->size; 
    int i = minHeap->size - 1; 
  
    while (i && Node->occurences < minHeap->arr[(i - 1) / 2]->occurences) { 
        minHeap->arr[i] = minHeap->arr[(i - 1) / 2]; 
        i = (i - 1) / 2; 
    } 
  
    minHeap->arr[i] = Node; 
} 
  
void buildMinHeap(struct MinHeap* minHeap) { 
  
    int n = minHeap->size - 1; 
    int i; 
  
    for (i = (n - 1) / 2; i >= 0; --i) {
        minHeapify(minHeap, i); 
    }
} 
  
void printArr(int arr[], int n, int fd) { 

    int i; 
    for (i = 0; i < n; ++i) {


        char binary [sizeof(arr[i])];
        sprintf(binary, "%d", arr[i]); 

        int size = write(fd, binary, strlen(binary));
        checkFDError(size);
    }

} 
  
int isLeaf(struct Node* root) { 
    return !(root->left) && !(root->right); 
} 
  

struct MinHeap* createAndBuildMinHeap(Token** tokens, int size) { 
  
    struct MinHeap* minHeap = createMinHeap(size); 
    int i;
    for (i = 0; i < size; ++i) {
        minHeap->arr[i] = newNode(tokens[i]->data, tokens[i]->occurences); 
    }
  
    minHeap->size = size; 
    buildMinHeap(minHeap); 
  
    return minHeap; 
} 
  
struct Node* buildHuffmanTree(Token** tokens, int size) { 
    struct Node *left, *right, *top; 
    struct MinHeap* minHeap = createAndBuildMinHeap(tokens, size); 
  
    while (!isSizeOne(minHeap)) { 

        left = extractMin(minHeap); 
        right = extractMin(minHeap); 

        top = newNode("$", left->occurences + right->occurences); 
  
        top->left = left; 
        top->right = right; 
  
        insertMinHeap(minHeap, top); 
    } 
  

    return extractMin(minHeap); 
} 
  
int createHeader(int fd){
    int size;
    size = write(fd, space, 4);
    checkFDError(size);
    size = write(fd, " ", 1);
    checkFDError(size);
    size = write(fd, tab, 4);
    checkFDError(size);
    size = write(fd, " ", 1);
    checkFDError(size);
    size = write(fd, newline, 4);
    checkFDError(size);
    size = write(fd, "\n", 1);
    checkFDError(size);
    return fd;
    
}

void printCodes(struct Node* root, int arr[], int top, int fd){ 
  
    if (root->left) { 
        arr[top] = 0; 
        printCodes(root->left, arr, top + 1, fd); 
    } 
  
    if (root->right) { 
        arr[top] = 1; 
        printCodes(root->right, arr, top + 1, fd); 
    } 
  

    if (isLeaf(root)) { 
        int size;
        printArr(arr, top, fd); 
        size = write(fd, "\t", 1);
        checkFDError(size);
        size = write(fd, root->data, strlen(root->data));
        checkFDError(size);
        size = write(fd, "\n", 1);
        checkFDError(size);
        
    } 
} 

void freeTree(struct Node* root){
    if(root == NULL){
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}
  


void HuffmanCodes(Token** tokens, int size, int fd) { 
    struct Node* root = buildHuffmanTree(tokens, size); 
    int arr[100], top = 0; 
    
    fd = createHeader(fd);
    printCodes(root, arr, top, fd); 

    freeTree(root);
} 

int isDelimeter(char c){
    if(c == ' ' || c == '\n' || c == '\t' || c == EOF){
        return 1;
    }else{
        return 0;
    }
}



int insertArr(Token** tokens,  char* tkn, int arr_idx){
    int i;
    for(i=0; i<arr_idx;i++){
        if (strcmp(tokens[i]->data,tkn) == 0){
            tokens[i]->occurences++;
            return arr_idx;
        }
    }
    tokens[arr_idx]->data = tkn;
    tokens[arr_idx]->occurences = 1;
    arr_idx++;
    return arr_idx;
}

void freeTokens(Token** tokens, int size){
    int i;
    for(i = 0; i < size; i++){
        free(tokens[i]);
    }
    free(tokens);
}

int isCompressedFile(char* name){
    if(name[strlen(name) - 4] != '.' || name[strlen(name) - 3] != 'h' || name[strlen(name) - 2] != 'c' || name[strlen(name) - 1] != 'z'){
        return 0;
    }else{
        return 1;
    }

}

int isDirectory(char* name){
    int i;
    for(i = 0; i < strlen(name); i++ ){
        if(i!= 0 && i != 1 && name[i] == '.'){
            return 0;
        }
    }
    return 1;
}



int tokenSearch(char* search, Token** tokens, int size){
    int i;
    for(i = 0; i < size; i++){

        if(strcmp(search, tokens[i]->data) == 0){
                                

            return 1;
        }
    }
    return 0;
}



void printCodeToFile(struct Node* root, int arr[], int top,int fd, char* token){ 
  
    if (root->left) { 
        arr[top] = 0; 
        printCodeToFile(root->left, arr, top + 1, fd, token); 
    } 
  
    if (root->right) { 
        arr[top] = 1; 
        printCodeToFile(root->right, arr, top + 1, fd,token); 
    } 
  

    if (!strcmp(root->data,token)) { 
        printArr(arr, top, fd);     
    } 
} 

void tokenEncoder(Node* root, char* token, int fd){
    int arr[100], top=0; 
    printCodeToFile(root, arr, top, fd, token);
    
    return;
}

void compressFile(char* file_location, int cd){
    int i;
    int fd = open(file_location, O_RDONLY);
    if(fd < 0){
        perror("Error opening the file to compress.\nResponse: FATAL ERROR");
        exit(1);
    }

    char compressedFileLoc [100];
    strcpy(compressedFileLoc, file_location);
    strcat(compressedFileLoc, ".hcz");
    int fd_compressed = open(compressedFileLoc, O_WRONLY | O_CREAT, 0777);
    if(fd < 0){
        perror("Error creating the compressed file.\nResponse: FATAL ERROR");
        exit(1);
    }

    Node* root = newNode("", 0);
    root = recreateHuffmanTree(cd, root); 
    char* read_in_buff = malloc(1000 * sizeof(char));
    int num_bytes;

    int token_size = 100;
    char token[token_size];
    int token_idx = 0;
    while(1){
        num_bytes = read(fd, read_in_buff, 1000);
        if(num_bytes < 0){
            exit(1);
        }

        for(i = 0; i < num_bytes; i++){
            
            if(isDelimeter(read_in_buff[i]) == 0){
                token[token_idx] = read_in_buff[i];
                token_idx++;
                if(token_idx == token_size){
                    token_size = expand_buffer(token, token_size);
                }
            }else{
                if(strcmp(token, "") != 0){
                    token[token_idx] = '\0';


                    char temp [token_idx];
                    strcpy(temp, token);

                    tokenEncoder(root, temp, fd_compressed);

                    if(read_in_buff[i] == ' '){
                        tokenEncoder(root, space, fd_compressed);
                    }else if(read_in_buff[i] == '\t'){
                        tokenEncoder(root, space, fd_compressed);
                    }else{
                        tokenEncoder(root, newline, fd_compressed);
                    }



                    memset(token, 0, sizeof(token));
                    token_idx = 0;
                }
            }

            if(num_bytes < 1000 && i == (num_bytes - 1)){
                char temp [token_idx];
                strcpy(temp, token);

                tokenEncoder(root, temp, fd_compressed);

                if(read_in_buff[i] == ' '){
                    tokenEncoder(root, space, fd_compressed);
                }else if(read_in_buff[i] == '\t'){
                    tokenEncoder(root, space, fd_compressed);
                }else{
                    tokenEncoder(root, newline, fd_compressed);
                }
                memset(token, 0, sizeof(token));
                token_idx = 0;
                break;

                
            }


        }
        break;

    }
    free(read_in_buff);
    freeTree(root);
    close(fd);
    close(fd_compressed);
}

int tokenizeFile(char* filename, Token** tokens, int numTokens){
     int i;
            


    int fd = open(filename, O_RDONLY);
    char* read_in_buff = malloc(1000 * sizeof(char));
    int num_bytes;

    int token_size = 100;
    char token[token_size];
    int token_idx = 0;


    

    while(1){
        num_bytes = read(fd, read_in_buff, 1000);
        if(num_bytes < 0){
            exit(1);
        }

        for(i = 0; i < num_bytes; i++){
            
            if(isDelimeter(read_in_buff[i]) == 0){
                token[token_idx] = read_in_buff[i];
                token_idx++;
                if(token_idx == token_size){
                    token_size = expand_buffer(token, token_size);
                }
            }else{
                if(strcmp(token, "") != 0){
                    token[token_idx] = '\0';


                    char* temp = malloc(token_idx * sizeof(char));
                    strcpy(temp, token);

                    numTokens = insertArr(tokens, temp, numTokens);

                    if(strcmp(token, "ssss") == 0){
                        change_space = 1;
                    }
                    if(strcmp(token, "tttt") == 0){
                        change_tab = 1;
                    }
                    if(strcmp(token, "nnnn") == 0){
                        change_newline = 1;
                    }



                    memset(token, 0, sizeof(token));
                    token_idx = 0;
                }


                if(read_in_buff[i] == ' '){
                    space_counter++;
                    

                }else if(read_in_buff[i] == '\t'){
                    tab_counter++;

                }else if(read_in_buff[i] == '\n'){
                    newline_counter++;
                }
            }

            if(num_bytes < 1000 && i == (num_bytes - 1)){
                char* temp = malloc(token_idx * sizeof(char));
                strcpy(temp, token);
                numTokens =insertArr(tokens, temp, numTokens);
                
                if(strcmp(token, "ssss") == 0){
                    change_space = 1;
                }
                if(strcmp(token, "tttt") == 0){
                    change_tab = 1;
                }
                if(strcmp(token, "nnnn") == 0){
                    change_newline = 1;
                }
                memset(token, 0, strlen(token));
                if(isDelimeter(read_in_buff[i]) == 1){

                    if(read_in_buff[i] == ' '){
                        space_counter++;

                    }else if(read_in_buff[i] == '\t'){
                        tab_counter++;

                    }else if(read_in_buff[i] == '\n'){
                        newline_counter++;
                    }
                    break;
                }

                
            }

        }
        
        break;
    }
    free(read_in_buff);
    close(fd);
    return numTokens;
}

void decompressFile(char* compressedFile, int cbd){
    int cfd = open(compressedFile, O_RDONLY);
    if(cfd < 0){
        perror("Error opening compressedfile.\nResponse: FATAL ERROR");
        exit(1);
    }
    int i;

    char uncompressedFileLoc[100];
    for(i = 0; i < strlen(compressedFile) - 4; i++){
        uncompressedFileLoc[i] = compressedFile[i];
    }    
    uncompressedFileLoc[i] = '\0';
    
    
    Node* root = newNode("", 0);
    root = recreateHuffmanTree(cbd, root);
    Node* currNode = root;
            

    int ufd = open(uncompressedFileLoc, O_WRONLY | O_CREAT, 0777);
    if(ufd < 0){
        perror("Error opening compressedfile.\nResponse: FATAL ERROR");
        exit(1);
    }
    int size;
    char* read_in_buff = malloc(1000 * sizeof(char));
    int num_bytes;

    while(1){
        num_bytes = read(cfd, read_in_buff, 1000);
        if(num_bytes < 0){
            exit(1);
        }

        for(i = 0; i < num_bytes; i++){
            if(read_in_buff[i] == '0'){
                currNode = currNode->left;
            }else if(read_in_buff[i] == '1'){
                currNode = currNode->right;
            }else{
                break;
            }

            if(currNode->left == NULL && currNode->right == NULL){
                if(strcmp(currNode->data, space) == 0){
                    size = write(ufd, " ", 1);
                }else if(strcmp(currNode->data, tab) == 0){
                    size = write(ufd, "\t", 1);
                }else if(strcmp(currNode->data, newline) == 0){
                    size = write(ufd, "\n", 1);
                }else{
                    size = write(ufd, currNode->data, strlen(currNode->data));

                }

                currNode = root;
            }
            
        }
        
        break;
    }
    free(read_in_buff);
    close(ufd);
    close(cbd);
    close(cfd);

}


void recursiveOp(char *name, int indent, char op){
    DIR *dir;
    struct dirent *entry;


    if (!(dir = opendir(name))){
        perror("Error opening the directory you specified or a subdirectory.\nResponse: FATAL ERROR\n");
        exit(-1);
    }
    
    
    while ((entry = readdir(dir)) != NULL) {

        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            recursiveOp(path, indent + 2, op);
        } else {
                if (strcmp(entry->d_name, "huffmanCoding.c") == 0 || strcmp(entry->d_name, "a.out") == 0 || strcmp(entry->d_name, "HuffmanCodebook") == 0){
                    perror("Error one of your files will not be operated on because it is either this file, a Huffman Codebook, an already compressed file, or an executable.\nResponse: It will be ignored");
                    continue;
                }else{
                    char file_location[1024];
                    strcpy(file_location, name);
                    strcat(file_location, "/");
                    strcat(file_location, entry->d_name);

                    if(op == 'b'){
                        if(isCompressedFile(entry->d_name) == 1 ){
                            perror("Error: one of your files is already compressed, we will not add it to Huffman Codebook\n");
                            continue;
                        }
                        numTokens = tokenizeFile(file_location, tokens, numTokens);
                    }else if(op == 'c'){
                        if(isCompressedFile(entry->d_name) == 1 ){
                            perror("Error: one of your files is already compressed, we will not add it to Huffman Codebook\n");
                            continue;
                        }
                        compressFile(file_location, codebookWriter);
                    }else{
                        if(isCompressedFile(entry->d_name) == 0){
                            perror("Error: a file we found was uncompressed. We will not touch it\n");
                            continue;
                        }
                        decompressFile(file_location, codebookWriter);
                    }
                    
                    
                }
                     
        }
    }

    closedir(dir);
}
  
int main(int argc, char** argv) { 

    char flag [2];
    flag[0] = '0';
    flag[1] = '0';
    char* fileLoc;
    char* codebookLoc;
    int i;

    if(argc < 3 || argc > 5){
        perror("Error in the amount of command line arguments, please your flags and the appropriote amount of file/directory names to perform the operation.\nResponse: FATAL ERROR\n");
        exit(-1);
    }

    int readingFlags = 1;
    int flagsToRead = 1;
    int filesToRead = 1;
    int flagIdx = 0;
    int codebookNeeded = 0;
    for(i = 1; i < argc; i++){
        char* input = argv[i];
        if(flagsToRead  > 0){
            if(input[0] != '-'){
                perror("Error in the flags you specified, please add a - before your flag to specify that it is a flag.\nResponse: FATAL ERROR\n");
                exit(-1);
            }
            if(input[1] != 'R' && input[1] != 'd' && input[1] != 'b' && input[1] != 'c'){
                perror("Error in the flags you specified, please use either R, d, b, or c.\nResponse: FATAL ERROR\n");
                exit(-1);
            }

            if(i == argc - 1){
                perror("You must specify one or two file/directory locations, depending on the mode.\nResponse: FATAL ERROR\n");
                exit(-1);
            }

            if(input[1] == 'R'){
                if(flagIdx != 0){
                    perror("Error in the ordering of the flags you specified.\nResponse: will continue as if they were ordered correctly\n");
                    char temp = flag[0];
                    flag[0] = 'R';
                    flag[1] = temp;
                    flagsToRead = 0;
                }
                flag[flagIdx] = 'R';
                flagIdx++;
                continue;
            }else{
                if(input[1] != 'b'){
                    codebookNeeded = 1;
                }
                flag[flagIdx] = input[1];
                flagsToRead--;
                flagIdx++;
                break;
            }
            
        }
    }

    if(codebookNeeded == 1){
        fileLoc = argv[3];
        codebookLoc = argv[4];
    }else{
        fileLoc = argv[3];
    }
        

    if(flag[0] != 'R' && flag[0] != 'd' && flag[0] != 'c' && flag[0] != 'b'){
        perror("Error in the flags you specified.\nResponse: FATAL ERROR\n");
        exit(-1);
    }

    if(flag[0] == 'R' && !flag[1]){
        perror("Error in the flags you specified.\nResponse: FATAL ERROR\n");
        exit(-1);
    }

    if((flag[0] == 'd' || flag[1] == 'd') && !codebookLoc){
        perror("Error: you specified decode mode but did not provide a codebook location.\nResponse: FATAL ERROR\n");
        exit(-1);
    }

    

    int maxSize=1000;
    tokens = malloc(maxSize * sizeof(Token*));
    for(i = 0; i < maxSize; i++){
        tokens[i] = (Token*)malloc(sizeof(Token));
    }
    
    if(flag[0] == 'R'){

        if(isDirectory(fileLoc) == 0){
            perror("Error: you must specify a directory to navigate through.\nResponse: FATAL ERROR\n");
            exit(-1);
        }

        codebookWriter = open("HuffmanCodebook", O_WRONLY | O_CREAT, 0777);

        if(codebookWriter < 0){
            perror("Error creating the Huffman Codebook.\nResponse: FATAL RESPONSE\n");
            exit(1);
        }
        if(flag[1] == 'b'){
            recursiveOp(fileLoc, 0, 'b');
            
            numTokens = fixDelimeters(tokens, numTokens);
            quickSort(tokens, 0, numTokens - 1);
            HuffmanCodes( tokens, numTokens, codebookWriter); 
        }else if(flag[1] == 'c'){
            codebookWriter = open(codebookLoc, O_RDONLY);
            if(codebookWriter < 0){
                perror("Error finding/opening the Huffman Codebook.\nResponse: FATAL RESPONSE\n");
                exit(1);
            }
            recursiveOp(fileLoc, 0, 'c');
        }else{
            codebookWriter = open(codebookLoc, O_RDONLY);
            if(codebookWriter < 0){
                perror("Error finding/opening the Huffman Codebook.\nResponse: FATAL RESPONSE\n");
                exit(1);
            }
            recursiveOp(fileLoc, 0, 'd');
        }
        
        

    }else{
        if(flag[0] == 'b'){
            codebookWriter = open("HuffmanCodebook", O_WRONLY | O_CREAT, 0777);
            if(codebookWriter < 0){
                perror("Error creating the Huffman Codebook file.\nResponse: FATAL RESPONSE\n");
                exit(1);
            }
            numTokens = tokenizeFile(fileLoc, tokens, numTokens);
            quickSort(tokens, 0, numTokens - 1);
            numTokens = fixDelimeters(tokens, numTokens);
            HuffmanCodes(tokens, numTokens, codebookWriter);
        }else if(flag[0] == 'c'){
            codebookWriter = open(codebookLoc, O_RDONLY);
            if(codebookWriter < 0){
                perror("Error finding/opening the Huffman Codebook.\nResponse: FATAL RESPONSE\n");
                exit(1);
            }
            compressFile(fileLoc, codebookWriter);
        }else{
            codebookWriter = open(codebookLoc, O_RDONLY);
            if(codebookWriter < 0){
                perror("Error finding/opening the Huffman Codebook.\nResponse: FATAL RESPONSE\n");
                exit(1);
            }
            decompressFile(fileLoc, codebookWriter);
        }
        

    }

    freeTokens(tokens, maxSize);


    
    return 0; 
} 