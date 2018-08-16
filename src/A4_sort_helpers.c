#include "A4_sort_helpers.h"
sem_t * semArray[27];

// Function: read_all() 
// Provided to read an entire file, line by line.
// No need to change this one.
void read_all( char *filename ){
    
    FILE *fp = fopen( filename, "r" );
    int curr_line = 0;
	
    while( curr_line < MAX_NUMBER_LINES && 
           fgets( text_array[curr_line], MAX_LINE_LENGTH, fp ) )
    {
        curr_line++;
    }
	
    text_array[curr_line][0] = '\0';
    fclose(fp);
}

// Function: read_all() 
// Provided to read only the lines of a file staring with first_letter.
// No need to change this one.
void read_by_letter( char *filename, char first_letter ){

    FILE *fp = fopen( filename, "r" );
    int curr_line = 0;
    text_array[curr_line][0] = '\0';
	
    while( fgets( text_array[curr_line], MAX_LINE_LENGTH, fp ) ){
        if( text_array[curr_line][0] == first_letter ){
            curr_line++;
        }

        if( curr_line == MAX_NUMBER_LINES ){
            sprintf( buf, "ERROR: Attempted to read too many lines from file.\n" );
            write( 1, buf, strlen(buf) );
            break;
        }
    }
	
    text_array[curr_line][0] = '\0';
    fclose(fp);
}

//Implemented bubblesort
void sort_words( ){
	
	int numOfLines= 0;
	while( text_array[numOfLines][0] != '\0' ){
		numOfLines++;
	}
	
	for(int i=0;i<=numOfLines-1;i++){
		for(int j=i+1;j<=numOfLines-1;j++){
			if(strcmp(text_array[i],text_array[j]) > 0){
				for(int n=0;n<MAX_LINE_LENGTH;n++){
					char temp=text_array[i][n];
					text_array[i][n]=text_array[j][n];
					text_array[j][n]=temp;
				}	
			}
		}
	}
}

int initialize( ){
	//Initialise semaphores
     for(int i=0;i<27;i++){
	char string[]= "Sort_ " ;
	string[5]='a'+i;
	sem_unlink(string);
	
	semArray[i]=sem_open( string, O_CREAT, 0666, 0 );
	}

	//Posts to the first semaphore so that the program can start
	sem_post(semArray[0]);
    return 0;
}
  
int process_by_letter( char* input_filename, char first_letter ){
    
    sem_wait(semArray[first_letter-'a']);
    sprintf( buf, "This process will sort the letter %c.\n",  first_letter );
    write(1,buf,strlen(buf));
    read_by_letter( input_filename, first_letter );
     sort_words( );

 	//Writes to file
	FILE *fp = fopen( "TempFile.txt", "a" );
	int curr_line = 0;
	while( text_array[curr_line][0] != '\0' ){
		sprintf( buf, "%s",  text_array[curr_line] );
		fprintf(fp,"%s",buf);
		curr_line++;
	}
	
	sem_post(semArray[first_letter-'a'+1]);
    return 0;
}

int finalize( ){

	//Calls wait to prevent inconsistencies
	sem_wait(semArray[26]);
	
	//Closing all semaphores
	for(int i=0;i<27;i++){
		char string[]= "Sort_ " ;
	string[5]='a'+i;
	sem_unlink(string);
		sem_close(semArray[i]);
	}

	//Reading from file and writing to standard output
	read_all("TempFile.txt"  );
	int curr_line = 0;
	while( text_array[curr_line][0] != '\0' ){
		sprintf( buf, "%s",  text_array[curr_line] );
		write(1,buf,strlen(buf));
		curr_line++;
	}

	sprintf( buf, "Sorting complete!\n" );
	write( 1, buf, strlen(buf) );
	
    return 0;
}
