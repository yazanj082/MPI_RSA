#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <mpi.h>
#include <setjmp.h>

// Function prototypes
void setkeys();
long long int encrypt(double message);
long long int decrypt(int encrypted_text);
long long * encoder(const char* message, int* size);
char* decoder(const long long int* encoded, int size);

// Global variables
long long  public_key;
long long  private_key;
long long n;
int rank, size,arraySize;


int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

// Function to set public and private keys

void SetPublicKey(long long  prime1,long long  prime2){
    n = prime1 * prime2;
    int e = 2;
    long long  fi = (prime1 - 1) * (prime2 - 1);
    int e1 = e + rank;
    int buff = 0,snd_buff=0 ,count =0,flag = 0;
    while (1) {
        if (gcd(e1, fi) == 1)
        {
            flag = 1;
            snd_buff = e1;
        }
        if(flag == 1 || count%50 ==0)
        {
            MPI_Allreduce(&snd_buff,&buff,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
            if(buff > 0)
                break;
        }
        e1 += size;
        count +=1;
    }
    public_key = buff;
}
void SetPrivateKey(long long  prime1,long long  prime2) {
    int d = 2;
    long long  fi = (prime1 - 1) * (prime2 - 1);
    int d1 = d + rank;
    int buff = 0, snd_buff=0 ,count =0,flag = 0;
    while (1) {
        if ((d1 * public_key) % fi == 1)
        {
            flag = 1;
            snd_buff = d1;
        }

        if(flag == 1 || count%100 ==0)
        {
            MPI_Allreduce(&snd_buff,&buff,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
            if(buff > 0)
                break;
        }
        d1 += size;
        count +=1;
    }
    private_key = buff;
}
void setkeys(long long prime1,long long prime2){
    SetPublicKey(prime1,prime2);
    SetPrivateKey(prime1,prime2);

}
// Function to encrypt the given number
long long int encrypt(double message) {
    long long int *resultArr = (long long int*)malloc(size  * sizeof(long long int));
    
    long long int encrypted_text = 1,result = 1;
    if(rank < public_key){
        
    int section = public_key/size;
    if(size >public_key)
    section = 1;
    int start = section*rank;
    int end = start + section;
    if(rank == size -1){
        
        end += public_key%size;
            
    }

    for (int i = end;i>start;i--) {
        
        encrypted_text *= message;
        
        encrypted_text %= n;
    }
    }
    //MPI_Allreduce(&encrypted_text, &result,1, MPI_INT64_T,MPI_PROD,MPI_COMM_WORLD);
    MPI_Allgather(&encrypted_text,1,MPI_INT64_T,resultArr,1,MPI_INT64_T,MPI_COMM_WORLD);
    result = encrypted_text;
    for (int i = 0;i<size;i++) {
        if (rank == i)
            continue;
        result *= resultArr[i];
        result %= n;
    }
    free(resultArr);
    
    return result;
}

// Function to decrypt the given number
long long int decrypt(int encrypted_text) {
    long long int *resultArr = (long long int*)malloc(size  * sizeof(long long int));
    long long int decrypted = 1,result = 1;
        if(rank < private_key){
        
    int section = private_key/size;
    if(size >private_key)
    section = 1;
    int start = section*rank;
    int end = start + section;
    if(rank == size -1){
        
        end += private_key%size;
            
    }
    for (int i = end;i>start;i--) {
        decrypted *= encrypted_text;
        decrypted %= n;
    }
        }
    //MPI_Allreduce(&decrypted, &result,1, MPI_INT64_T,MPI_PROD,MPI_COMM_WORLD);
    MPI_Allgather(&decrypted,1,MPI_INT64_T,resultArr,1,MPI_INT64_T,MPI_COMM_WORLD);
    result = decrypted;
    for (int i = 0;i<size;i++) {
        if (rank == i)
            continue;
        result *= resultArr[i];
        result %= n;
    }
    free(resultArr);
    return result;
}

// Function to encode the message
long long int* encoder(const char* message, int* size_arr) {
    *size_arr = strlen(message);
    long long int* form = (long long int*)malloc(*size_arr * sizeof(long long int));

    for (int i = 0; i < *size_arr; i++) {
        form[i] = encrypt((long long int)message[i]);
    }

    return form;
}

// Function to decode the message
char* decoder(const long long int* encoded, int size) {
    char* s = (char*)malloc((size + 1) * sizeof(char));

    for (int i = 0; i < size; i++) {
        s[i] = decrypt(encoded[i]);
    }

    s[size] = '\0';
    return s;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char* message = argv[1];
    
    setkeys(4001
    ,4003);
    int size;
    long long int* coded = encoder(message, &size);
    char* decoded = decoder(coded, size);
    if(rank == 0){
    //printf("the public key (%lld)\nthe private key (%lld)\n, the n is : (%lld)\n",public_key,private_key,n);
    
    printf("Initial message:\n%s\n\n", message);
    printf("The encoded message (encrypted by public key):\n");
    for (int i = 0; i < size; i++) {
        printf("%lld", coded[i]);
    }

    printf("\n\nThe decoded message (decrypted by private key):\n%s\n",decoded );
    }
    // Free allocated memory
    free(coded);
    free(decoded);
    MPI_Finalize();
    return 0;
}
