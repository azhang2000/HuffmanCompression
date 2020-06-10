#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

struct avlNode{                     // AVL Tree Node
    char * data;                    // String token
    char * bitCode;                 // Huffman Code representing token
    int occ;                        // Number of token occurrences in file 
    int height;                     
    struct avlNode * left;          
    struct avlNode * right;         
};

struct heapNode{                    // minHeap Node
    char * data;                    // String token
    int occ;                        // Number of token occurrences in file
    struct heapNode * left;
    struct heapNode * right;
};

struct avlNode * root;              // Global AVL Tree root node
struct heapNode *  minHeap;         // Global minHeap which will be built into the Huffman Codebook
int heapSize = 0;                   // Size of minHeap determined by size of AVL Tree
int heapIndex = 0;                  // Index used to fill minHeap, might not need 

// Helper methods for AVL Tree Balancing
int height(struct avlNode * root){
    return root == NULL ? 0 : root->height;
}

int max (int a, int b){
    return a > b ? a : b;
}

struct avlNode * newNode(char * data, char * bitCode){
    struct avlNode * newNode = (struct avlNode *)malloc(sizeof(struct avlNode));
    newNode->data = data;
    newNode->bitCode = bitCode;
    newNode->occ = 1;
    newNode->height = 0;
    newNode->left = NULL;
    newNode->right = NULL;
    heapSize++;
    return newNode;
}

struct avlNode * rightRotate(struct avlNode * prev) { 
    struct avlNode * root = prev->left; 
    struct avlNode * temp = root->right; 
  
    root->right = prev; 
    prev->left = temp; 
  
    prev->height = max(height(prev->left), height(prev->right))+1; 
    root->height = max(height(root->left), height(root->right))+1; 
  
    return root; 
} 
  
struct avlNode * leftRotate(struct avlNode * prev) { 
    struct avlNode * root = prev->right; 
    struct avlNode * temp = root->left; 
  
    root->left = prev; 
    prev->right = temp; 
  
    prev->height = max(height(prev->left), height(prev->right))+1; 
    root->height = max(height(root->left), height(root->right))+1; 
  
    return root; 
}

int rebalance(struct avlNode * root){
    return root == NULL ? 0 : height(root->left) - height(root->right);
}

// AVL Tree insert
struct avlNode * insert(struct avlNode * root, char * data, char * bitCode){
    if (root == NULL){
        heapSize++;
        return newNode(data, bitCode);
    }

    if(strcmp(data, root->data)>0){
        root->right = insert(root->right, data, bitCode);
    }
    else if(strcmp(data,root->data)<0){
        root->left = insert(root->left, data, bitCode);
    }
    else{
        root->occ++;
        return root;
    }
    root->height = 1 + max(height(root->left),height(root->right));
    int balance = rebalance(root);
    if (balance > 1 && strcmp(data, root->left->data) <0){ 
        return rightRotate(root); 
    }
    // Right Right Case 
    if (balance < -1 && strcmp(data, root->right->data)>0){ 
        return leftRotate(root); 
    }
    // Left Right Case 
    if (balance > 1 && strcmp(data,root->left->data)>0){ 
        root->left =  leftRotate(root->left); 
        return rightRotate(root); 
    } 
  
    // Right Left Case 
    if (balance < -1 && strcmp(data, root->right->data)<0){ 
        root->right = rightRotate(root->right); 
        return leftRotate(root); 
    }
    return root;
}
struct avlNode * d_insert(struct avlNode * root, char * data, char * bitCode){
    if (root == NULL){
        heapSize++;
        return newNode(data, bitCode);
    }

    if(strcmp(bitCode, root->bitCode)>0){
        root->right = d_insert(root->right, data, bitCode);
    }
    else if(strcmp(bitCode,root->bitCode)<0){
        root->left = d_insert(root->left, data, bitCode);
    }
    else{
        root->occ++;
        return root;
    }

    root->height = 1 + max(height(root->left),height(root->right));
    int balance = rebalance(root);
    
    if (balance > 1 && strcmp(bitCode, root->left->bitCode) <0){ 
        return rightRotate(root); 
    }
    if (balance < -1 && strcmp(bitCode, root->right->bitCode)>0){ 
        return leftRotate(root); 
    }
    if (balance > 1 && strcmp(bitCode,root->left->bitCode)>0) { 
        root->left =  leftRotate(root->left); 
        return rightRotate(root); 
    } 
    if (balance < -1 && strcmp(bitCode, root->right->bitCode)<0) { 
        root->right = rightRotate(root->right); 
        return leftRotate(root); 
    }   
    
    return root;
}


char * nToBinary(int n, int size){
    size++;
    char * binary = (char *)malloc(size * sizeof(char));
    size--;
    binary[size] = '\0';
    size--;
    int i;
    for (i = size; i >= 0; i--){
        int d = n >> i;
        if (d & 1){
            binary[i] = '1';
        }
        else {
            binary[i] = '0';
        }
    } 
    return binary;
}

// Function used to update bitCode for each token in tree after Huffman Codebook is built
void updateBit(struct avlNode * root, char * data, int bitCode, int size){
    if (root == NULL){
        printf("Error: %s  not found\n",data); 
        return;
    }

    if(strcmp(data, root->data)>0){
        updateBit(root->right, data, bitCode, size);
    }
    else if(strcmp(data,root->data)<0){
        updateBit(root->left, data, bitCode, size);
    }
    else{
        root->bitCode = nToBinary(bitCode,size);
    }
    return;
}
// Function used to compress tokens into their bitCode
char * compress(struct avlNode * root, char * data){
    if (root == NULL){
        printf("Error: Node not found\n");
        return NULL;
    }
    if(strcmp(data, root->data)>0){
        return compress(root->right, data);
    }
    else if(strcmp(data,root->data)<0){
        return compress(root->left, data);
    }
    else{
        return  root->bitCode;
    }
}
// minHeap insert
void fillHeap(char * data, int occ){
    minHeap[heapIndex].data = data;
    minHeap[heapIndex].occ = occ;
    heapIndex++;
}

void fillHeapFromTree (struct avlNode * root){
    if (root == NULL){
        return;
    }
    fillHeapFromTree(root->left);
    fillHeapFromTree(root->right);
    fillHeap(root->data, root->occ);
}

void freeAVL(struct avlNode * root){
    if (root == NULL){
        return;
    }
    freeAVL(root->left);
    freeAVL(root->right);
    if (root->data != NULL){
        free(root->data);
    }
    if (root->bitCode != NULL){
        free(root->bitCode);
	}
    free(root);
}
// Helper functions for minHeap
int getLeftChildIndex(int parentIndex){
    return 2 * parentIndex + 1;
}

int getRightChildIndex(int parentIndex){
    return 2 * parentIndex + 2;
}

int getParentIndex(int childIndex) {
    return (childIndex - 1) / 2;
}

int hasLeftChild(int index){
    return getLeftChildIndex(index) < heapSize;
}

int hasRightChild(int index){
    return getRightChildIndex(index) < heapSize;
}

int hasParent(int index){
    return getParentIndex(index) >= 0;
}

struct heapNode leftChild(int index){
    return minHeap[getLeftChildIndex(index)];
}

struct heapNode rightChild(int index){
    return minHeap[getRightChildIndex(index)];
}

struct heapNode parent(int index){
    return minHeap[getParentIndex(index)];
}

void swap(int indexOne, int indexTwo){
    struct heapNode temp = minHeap[indexOne];
    minHeap[indexOne] = minHeap[indexTwo];
    minHeap[indexTwo] = temp;
}

void heapifyUp(){
    int index = heapSize - 1;
    while(hasParent(index) && parent(index).occ > minHeap[index].occ){
        swap(getParentIndex(index),index);
        index = getParentIndex(index);
    }
}

void heapifyDown(int index){
    while(hasLeftChild(index)){
        int minChildIndex = getLeftChildIndex(index);
        if(hasRightChild(index) && rightChild(index).occ < leftChild(index).occ){
            minChildIndex = getRightChildIndex(index);
        }
        if (minHeap[index].occ < minHeap[minChildIndex].occ){
            break;
        }
        else{
            swap(index, minChildIndex);
        }
        index = minChildIndex;
    }
}

void heapify(){
    int index = heapSize - 1;
    if (hasParent(index)){
        index = getParentIndex(index);
    }
    else{
        return;
    }
    while (index >= 0){
        heapifyDown(index);
        index--;
    }
}

struct heapNode pop(){
    struct heapNode min = minHeap[0];
    swap(0,heapSize-1);
    heapSize--;
    heapifyDown(0);
    return min;
}

void add(struct heapNode newNode){
    minHeap[heapSize] = newNode;
    heapSize++;
    heapifyUp();
}
void buildfromCodebook(char * codebook, int flag){ //if flag = 0, avl sorted by token, if flag = 1, sorted by bitcode
    int i = 0;
    int start = 0;
    int esch = 0;
    char * eschar;
    while(codebook[i]!='\0'){
	    char * bitcode;
	    if(codebook[i]=='\t'){
	        int length = i-start;
	        bitcode = (char *)malloc(sizeof(char)*(length+1));
            int p =0;
            for(p =0; p<length;p++){
                bitcode[p]=codebook[start+p];
	        }
            bitcode[length]='\0';
            start=i+1;
	    } 
	    if(codebook[i]=='\n'){
	        int length = i-start;
	        char * token = (char *)malloc(sizeof(char)*(length+1));
	        token[length]='\0';
	        int p = 0;
	        for(p =0; p<length;p++){
                token[p]=codebook[start+p];
	        }
	        if(esch>0){
	            if(strstr(token,eschar)!=NULL  && strlen(token)==esch+1){
	                if(token[esch]=='s'){
		                if(flag == 0){
	                        root = insert(root," ",bitcode);
	                    }
		                else{
		                    root = d_insert(root," ",bitcode);
		                }
	                }
	                else if(token[esch]=='t'){
	                    if(flag == 0){
	                        root = insert(root,"\t",bitcode);
	                    }
		                else{
		                    root = d_insert(root,"\t",bitcode);
		                }
	                }
	                else if(token[esch]=='n'){
	                    if(flag == 0){
	                        root = insert(root,"\n",bitcode);
	                    }
		                else{
		                    root = d_insert(root,"\n",bitcode);
		                }
	                }
			else{
			  if(flag==0){
	                    root = insert(root,token,bitcode);
	                }
	                else{
	                    root = d_insert(root,token,bitcode);
	                }

			}
	            }
	            else {
	                if(flag==0){
	                    root = insert(root,token,bitcode);
	                }
	                else{
	                    root = d_insert(root,token,bitcode);
	                }
	            }
	        }
	        else{
	            eschar = token;
	            esch = length;
	        }
	        start = i+1;
	    }
	    i++;
    }
}

// Function used to transform minHeap arr into Huffman Tree structure
void buildHuffmanTree(){
    while (heapSize>1){
        struct heapNode * newNode = (struct heapNode *)malloc(sizeof(struct heapNode));
        struct heapNode newLeftChild = pop();
        struct heapNode newRightChild = pop();
        newNode->data = NULL;
        newNode->left = (struct heapNode *)malloc(sizeof(struct heapNode));
        newNode->left->data = newLeftChild.data;
        newNode->left->occ = newLeftChild.occ;
        newNode->left->left = newLeftChild.left;
        newNode->left->right = newLeftChild.right;
        newNode->right = (struct heapNode *)malloc(sizeof(struct heapNode));
        newNode->right->data = newRightChild.data;
        newNode->right->occ = newRightChild.occ;
        newNode->right->left = newRightChild.left;
        newNode->right->right = newRightChild.right;
        newNode->occ = newNode->left->occ + newNode->right->occ;
        add(*newNode);
    }
}
// Function used to traverse Huffman Tree and assign bitCodes to each token ( 0 for left, 1 for right )
void buildBitCode(struct heapNode * root, struct avlNode * tree, int bitCode, int size){
    if(root==NULL){
        return;
    }
    if (root->data != NULL){
	    updateBit(tree, root->data, bitCode, size);
	    return;
    }
    
    buildBitCode(root->left, tree, (bitCode<<1), size+1);
    buildBitCode(root->right, tree, (bitCode<<1)+1, size+1);
    return;
}

void freeHeap(struct heapNode * root){
    if (root==NULL){
        return;
    }
    freeHeap(root->left);
    freeHeap(root->right);
    /*if (root->data!=NULL){
        free(root->data);
	}*/
    free(root);
}

void fillCodebook(struct avlNode * root, int fdwrite, char * esc){
    if (root == NULL){
        return;
    }
    fillCodebook(root->left,fdwrite, esc);
    fillCodebook(root->right,fdwrite, esc);
    write(fdwrite,root->bitCode,strlen(root->bitCode));
    write(fdwrite,"\t",1);
    if(strcmp(root->data," ")==0){
        write(fdwrite,esc,strlen(esc));
	    write(fdwrite,"s",1);
    }
    else if(strcmp(root->data,"\n")==0){
        write(fdwrite,esc,strlen(esc));
        write(fdwrite,"n",1);
    }
    else if(strcmp(root->data,"\t")==0){
        write(fdwrite,esc,strlen(esc));
	    write(fdwrite,"t",1);
    }
    else {
        write(fdwrite,root->data,strlen(root->data));
    }
    write(fdwrite,"\n",1);
}

// Function used to decompress bitCode into corresponding tokens by traversing through Huffman Tree
char * decompress(struct avlNode * root, char * bitCode){
    if (root == NULL){
        printf("Fatal Error: Bitcode does not exist\n");
        return NULL;
    }

    if(strcmp(bitCode, root->bitCode)>0){
        return decompress(root->right, bitCode);
    }
    else if(strcmp(bitCode,root->bitCode)<0){
        return decompress(root->left,bitCode);
    }
    else{
        return root->data;
    }

}

int isPresent(struct avlNode * root, char * data){
    if (root == NULL){
        return 0;
    }
    if(strcmp(data, root->data)>0){
        return isPresent(root->right, data);
    }
    else if(strcmp(data,root->data)<0){
        return isPresent(root->left, data);
    }
    else{
        return 1;
    }
}

char * readfile(int fd){		//reads file and returns a string of entire file
    char * buf = malloc(300*sizeof(char));
    memset(buf,'\0',300);
    int bsize = 300;
    int index = 0;
    int status= 0;
    do{
        status = read(fd, buf + index, 1);
        index += status;
        if(index>=bsize-1)
        {
            char * temp =(char *) malloc(bsize*sizeof(char));
            memcpy(temp, buf, bsize);
            free(buf);
            buf = (char *)malloc(bsize*2*sizeof(char));
            memset(buf,'\0',bsize*2);
            memcpy(buf,temp,bsize);
            free(temp);
            bsize=bsize*2;
        }
    }while(status>0);

    return buf;
}

void countOccurrences(char * buf){ //takes buffer string and looks for tokens delimited by space, then calls insert method for each token
    int i =0;
    int start=0;
 
    do{
        if(buf[i]==' ' || buf[i]=='\n' || buf[i]=='\0'){
            int length = i-start;
            if(buf[i]==' '){
	            char * token = (char *)malloc(sizeof(char)*2);
	            token[1]='\0';
	            token[0]=' ';
		        root = insert(root, token, NULL);
	        }
            else if(buf[i]=='\n'){
	            char * token = (char *)malloc(sizeof(char)*2);
	            token[1]='\0';
	            token[0]='\n';
	            root = insert(root, token, NULL);
	        }
            if(length>0){
                char * word=(char * )malloc((length+1)*sizeof(char));
                int p =0;
                for(p =0; p<length;p++){
                    word[p]=buf[start+p];
                }
                //printf("%s\n",word);
                word[length]='\0';
                root = insert(root, word, NULL);
            }
            start=i+1;
        }
        i++;
    }while(buf[i-1]!='\0');
}
 
void printAVL(struct avlNode * root){
    if( root==NULL){
        return;
    }
    printAVL(root->left);
   
    if (root->data[0] == '\n'){
        printf("%s \\n %d\n", root->bitCode, root->occ);
    }
    else if (root->data[0] == ' '){
        printf("%s \\s %d\n", root->bitCode, root->occ);
    }
    else if (root->data[0] == '\t'){
        printf("%s \\t %d\n", root->bitCode, root->data, root->occ);
    }
    else {
        printf("%s %s %d\n", root->bitCode, root->data, root->occ);
    }
    printAVL(root->right);
}

void compressAll(char * buf, int fd){
    int i = 0;
    int start = 0;
    do{
        if(buf[i]==' ' || buf[i]=='\n' || buf[i]=='\t' ||buf[i]=='\0'){
            int length = i-start;
            if(length>0){
                char * word=(char * )malloc((length+1)*sizeof(char));
                int p =0;
                for(p =0; p<length;p++){
                    word[p]=buf[start+p];
                }
                word[length]='\0';
                char * bcode = compress(root,word);
		if(bcode!=NULL){
		  write(fd,bcode,strlen(bcode)+1);
		}

            }
	        if(buf[i]==' '){
	            char * bcode = compress(root," ");
	            write(fd,bcode,strlen(bcode)+1);
	        }
            else if(buf[i]=='\n'){
	            char * bcode = compress(root,"\n");
	            write(fd,bcode,strlen(bcode)+1);
	        }
	    else if(buf[i]=='\t'){
	            char * bcode = compress(root,"\t");
	            write(fd,bcode,strlen(bcode)+1);
	        }
		
		start=i+1;
        }
        i++;
    } while(buf[i-1]!='\0');
}

void uncompressAll(int fdread, int fdwrite){
    char * buf = malloc(300*sizeof(char));
    memset(buf,'\0',300);
    int bsize = 300;
    int index = 0;
    int status= 0;
    do{
        status = read(fdread, buf + index, 1);
        index += status;
        if(index>=bsize-1)
        {
            char * temp =(char *) malloc(bsize*sizeof(char));
            memcpy(temp, buf, bsize);
            free(buf);
            buf = (char *)malloc(bsize*2*sizeof(char));
            memset(buf,'\0',bsize*2);
            memcpy(buf,temp,bsize);
            free(temp);
            bsize=bsize*2;
        }
    }while(status>0);
    int filesize = index;
    int i = 0;
    int start = 0;
    while(i<filesize){
      if(buf[i]=='\0'){
            int length = i-start;
            if(length>0){
                char * bcode=(char * )malloc((length+1)*sizeof(char));
                int p =0;
                for(p =0; p<length;p++){
                    bcode[p]=buf[start+p];
                }
                bcode[length]='\0';
		        char * token = decompress(root,bcode);
		        write(fdwrite,token,strlen(token));
            }
            start=i+1;
        }
        i++;
    }
}

void recurseDir(char * flag, char * path){  //if -R flag exists, will recursively iterate through directory and subdirectory and call functions based on flags
    struct dirent * dp;
    DIR * dir = opendir(path);

    if(dir!=NULL){
        readdir(dir);
        readdir(dir);
        while((dp=readdir(dir))!=NULL){
            int subpathlen = strlen(path) + strlen(dp->d_name) +2 ;
            char * subpath = (char *)malloc(sizeof(char)*subpathlen);
            memset(subpath, '\0', subpathlen);
            memcpy(subpath,path,strlen(path));
            subpath[strlen(path)]='/';
            strcat(subpath, dp->d_name);
            //printf("%s\n",subpath);
		    if(dp->d_type==DT_REG){
                if(strcmp(flag,"-b")==0 && strstr(subpath,".hcz")==NULL && subpath[strlen(subpath)-1]!='~'){
                    int fd = open(subpath,O_RDONLY);
                    if(fd!=-1){
	                   char * filecontents = readfile(fd);
	                   countOccurrences(filecontents);
                    }
                }
		        else if(strcmp(flag,"-c")==0 && strstr(subpath,".hcz")==NULL && subpath[strlen(subpath)-1]!='~'){
		            int fd = open(subpath, O_RDONLY);
		            char * uncompressed = readfile(fd);
		            char * filename = strcat(subpath,".hcz");
		            int fd3 =  open(filename, O_RDWR|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR); 
		            compressAll(uncompressed,fd3);
		            close(fd);
		            close(fd3);
		            //free(filename);
		            free(uncompressed);
		        }
		        else if(strcmp(flag,"-d")==0 && strstr(subpath,".hcz")!=NULL &&  subpath[strlen(subpath)-1]!='~'){
		            char * filepath = (char*)malloc(sizeof(char)*(strlen(subpath)-3));
		            filepath[strlen(subpath)-4] = '\0';
		            int o = 0;
		            for(o=0;o<strlen(subpath)-4;o++){
			            filepath[o]=(subpath)[o];
		            }
		            //printf("%s\n",filepath);
		            int fd = open(subpath,O_RDONLY);
		            int fd3 =  open(filepath, O_RDWR|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR);
                    uncompressAll(fd,fd3);
		            close(fd);
		            close(fd3);
		            // free(filepath);
		        }
		    }
            if(dp->d_type == DT_DIR){
		        recurseDir(flag,subpath);
		    }
		    //free(subpath);
	    }
        closedir(dir);
    }
}
void printHeap(struct heapNode * root){
    if(root==NULL){
        return;
    }
    printf("%s %d\n ", root->data,root->occ);
    printHeap(root->left);
    printHeap(root->right);
}
int main (int argc, char ** argv){
    if(strcmp(argv[1],"-R")==0){
        if(strcmp(argv[2],"-c")==0 || strcmp(argv[2],"-d")==0){
            int fd2 = open(argv[4],O_RDONLY);
            if(fd2 == -1){
	            printf("Fatal Error: Codebook not found\n");
	            return 0;
            }
            char * codebook = readfile(fd2);
            close(fd2);
            if(strcmp(argv[2],"-c")==0){
                buildfromCodebook(codebook,0);
            }
            else{
	            buildfromCodebook(codebook,1);
	        }
            recurseDir(argv[2],argv[3]);
            free(codebook);
        }
        else if(strcmp(argv[2],"-b")==0){
	        recurseDir(argv[2],argv[3]);
	        minHeap = (struct heapNode *)malloc(heapSize * sizeof(struct heapNode));
            fillHeapFromTree(root);
            heapify();
            buildHuffmanTree();
            buildBitCode(minHeap, root, 0,0);
	    int fd2 = open("./HuffmanCodebook",O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
	    char * esc = (char *)malloc(256 * sizeof(char));
            memset(esc, '\0', 256);
            int escIndex = 0;
            while(escIndex < 256){
                esc[escIndex] = '\\';
                esc[escIndex+1] = 'n';
                if (isPresent(root,esc)){
                    escIndex++;
                    continue;
                }
                esc[escIndex+1] = 't';
                if (isPresent(root,esc)){
                    escIndex++;
                    continue;
                }
                esc[escIndex+1] = 's';
                if (isPresent(root,esc)){
                    escIndex++;
                    continue;
                }
                esc[escIndex + 1] = '\0'; 
                break;
            }
	        write(fd2,esc,escIndex+1);
	        write(fd2,"\n",1);
            fillCodebook(root, fd2, esc);
	        write(fd2,"\n",1);
            close(fd2);
	    //freeHeap(minHeap);
            free(esc);
            heapSize = 0;
            heapIndex = 0;
        }
        else{
	        printf("Fatal Error: Invalid Flags\n");
	        return 0;
        }
    }

    else if(strcmp(argv[1],"-b")==0){
        int fd = open(argv[2],O_RDONLY);

        if(fd==-1){
            printf("Fatal Error: File does not exist\n");
            return 0;
        }
        char * filecontents = readfile(fd);
	    if(strlen(filecontents)==0){
	        printf("Warning: File is empty\n");
	        int fd2 = open("./HuffmanCodebook",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	        close(fd2);
	        return 0;
	    }
        countOccurrences(filecontents);
        minHeap = (struct heapNode *)malloc(heapSize * sizeof(struct heapNode));
	    fillHeapFromTree(root);
        heapify();
        buildHuffmanTree();
	    //printHeap(minHeap);
        buildBitCode(minHeap, root, 0,0);
	    int fd2 = open("./HuffmanCodebook",O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
	    //printAVL(root);
        char * esc = (char *)malloc(256 * sizeof(char));
        memset(esc, '\0', 256);
        int escIndex = 0;
        while(escIndex < 256){
            esc[escIndex] = '\\';
            esc[escIndex+1] = 'n';
            if (isPresent(root,esc)){
                escIndex++;
                continue;
            }
            esc[escIndex+1] = 't';
            if (isPresent(root,esc)){
                escIndex++;
                continue;
            }
            esc[escIndex+1] = 's';
            if (isPresent(root,esc)){
                escIndex++;
                continue;
            }
            esc[escIndex + 1] = '\0'; 
            break;
        }
	    write(fd2,esc,escIndex+1);
	    write(fd2,"\n",1);
        fillCodebook(root, fd2, esc);
	    write(fd2,"\n",1);
	    close(fd2);
	    //free(bitCode);
        free(filecontents);
        free(esc);
	// freeHeap(minHeap);
        heapSize = 0;
        heapIndex = 0;
    }
    else if (strcmp(argv[1],"-c")==0){
        int fd = open(argv[2], O_RDONLY);
        int fd2 = open(argv[3],O_RDONLY);

        if(fd==-1){
	        printf("Fatal Error: File not found\n");
	        return 0;
        }
        if(fd2 == -1){
	        printf("Fatal Error: Codebook not found\n");
	        return 0;
        }
        char * uncompressed = readfile(fd);
        char * codebook = readfile(fd2);

        buildfromCodebook(codebook,0);
        char * filename = strcat(argv[2],".hcz");
        int fd3 =  open(filename, O_RDWR|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR); 
        //printAVL(root);
        compressAll(uncompressed,fd3);
        close(fd);
        close(fd2);
        close(fd3);
        free(codebook);
        free(uncompressed);
	//free(filename);
    }

    else if(strcmp(argv[1],"-d")==0){
        int fd = open(argv[2], O_RDONLY);
        int fd2 = open(argv[3],O_RDONLY);

        if(fd==-1){
	        printf("Fatal Error: File not found\n");
	        return 0;
        }
        if(fd2 == -1){
	        printf("Fatal Error: Codebook not found\n");
            return 0;
        }
        char * codebook = readfile(fd2);
        char * filepath = (char*)malloc(sizeof(char)*(strlen(argv[2])-3));
        filepath[strlen(argv[2])-4] = '\0';
        int o = 0;
        for(o=0;o<strlen(argv[2])-4;o++){
    	  filepath[o]=(argv[2])[o];
	    }
        buildfromCodebook(codebook,1);
        int fd3 =  open(filepath, O_RDWR|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR);
        uncompressAll(fd,fd3);
        free(codebook);
        //free(filepath);
        close(fd);
        close(fd3);
        close(fd2);
    }
    else{
	printf("Fatal Error: Not valid flag\n");
    }
    //freeAVL(root);
    return 0;
}
