#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<math.h>
#include<dirent.h>

#define EXIT_CODE 0
#define true 1
#define false 0
#define SENTINEL_VALUE -1
#define MAX_FILE_SIZE 1000

typedef int bool;

/**
 * STRUCTURE PROCESS
 *
 * proc_id - auto incrementing primary key.
 * arrival_time - time of process' arrival.
 * burst_time - time for process to run to completion.
 * remaining_time - time left for process to run for.
 * completion_time - time process completes at.
 * turn_around_time - total amount of time from process arrival to completion.
 * wait_time - time process is in system without work being done upon it.
 **/
typedef struct Proc{
    int proc_id, arrival_time, burst_time, remaining_time, completion_time, turn_around_time, wait_time;
}Process;


/**
 * STRUCTURE FILE
 *
 * file_id - int for id of file
 * file_name - string for name of file
 **/
typedef struct Fl{
    int file_id;
    char *file_name;
}File;

 //*****************************************************************************************************

bool run_prg(int prg_count);
int get_num_valid_files();
void read_valid_files(File *files_ptr);
char *get_file_choice(File *files_ptr, int num_files);
char *get_file_path();
char *read_file(char *file_name);
void create_processes(char *file_text, Process *processes_ptr);
int get_num_processes(char *file_text);
void print_created_processes_output(Process *processes_ptr, int num_processes);
int algorithm_menu();
int get_sum_of_burst_times(Process *processes_ptr, int num_processes);


/** Scheduling Functions **/
void first_come_first_served(Process *processes_ptr, int num_processes);
void shortest_job_first(Process *processes_ptr, int num_processes);
void round_robin(Process *processes_ptr, int num_processes);
int tq_menu();
bool sentinel_values_formatted(int *proc_pid_queue_ptr, int num_processes);

/** Outcome Functions **/
void print_outcome_table(Process *processes_ptr, int num_processes);
void print_averages(Process *processes_ptr, int num_processes);

//*****************************************************************************************************
/**
 * description: Main method.
 * returns:     EXIT_CODE.
 **/
int main(int argc, char* argv[]){

	int prg_count = 0;

	int num_files;
	File *files_ptr;
	char *file_choice;

    char *file_text;
    int num_procs;
    Process *processes_ptr;


	while(run_prg(prg_count)){

        num_files = get_num_valid_files();
        files_ptr = malloc(num_files * sizeof(File));
        read_valid_files(files_ptr);
        file_choice = get_file_choice(files_ptr, num_files);

		file_text = read_file(file_choice);
		num_procs = get_num_processes(file_text);
        processes_ptr = malloc (num_procs * sizeof (Process));
		create_processes(file_text, processes_ptr);
		print_created_processes_output(processes_ptr, num_procs);

		switch(algorithm_menu()){
			case 1:
				first_come_first_served(processes_ptr, num_procs);
				print_outcome_table(processes_ptr, num_procs);
				print_averages(processes_ptr, num_procs);
				break;

			case 2:
				shortest_job_first(processes_ptr, num_procs);
				print_outcome_table(processes_ptr, num_procs);
				print_averages(processes_ptr, num_procs);
				break;

			case 3:
				round_robin(processes_ptr, num_procs);
				print_outcome_table(processes_ptr, num_procs);
				print_averages(processes_ptr, num_procs);
				break;

			default:
				printf("\nInvalid Selection\n");
				break;
		}

		free(files_ptr);
		free(processes_ptr);
		free(file_text);

		prg_count++;
	}

	return EXIT_CODE;
}

//*****************************************************************************************************

/**
 * description: Reads files in this directory counting the number of
 *              readable files for this program.
 * params:      files_ptr
 * returns:     num_files
 **/
int get_num_valid_files(){
    int num_files = 0;
    struct dirent *de;  // Pointer for directory entry
    DIR *dr = opendir(".");  // opendir() returns a pointer of DIR type.

    if (dr == NULL){  // opendir returns NULL if couldn't open directory
        printf("\nCould not open current directory\n" );
        return 0;
    }
    while ((de = readdir(dr)) != NULL){
            char *filename = de->d_name;
            char *csv_suffix = ".csv";
            char *txt_suffix = ".txt";
            if( (strstr(filename,csv_suffix) != NULL) || (strstr(filename,txt_suffix) != NULL) ){
                 num_files++;
            }
    }
    closedir(dr);
    return num_files;
}

/**
 * description: Reads files in this directory and populates files_ptr with values
 * params:      files_ptr
 * returns:     void
 **/
void read_valid_files(File *files_ptr){
    struct dirent *de;  // Pointer for directory entry
    DIR *dr = opendir(".");   // opendir() returns a pointer of DIR type.

    if (dr == NULL){  // opendir returns NULL if couldn't open directory
        printf("\nCould not open current directory\n" );
        return;
    }

    int file_num = 0;
    while ((de = readdir(dr)) != NULL){

            char *filename = de->d_name;
            char *csv_suffix = ".csv";
            char *txt_suffix = ".txt";


            if( (strstr(filename,csv_suffix) != NULL) || (strstr(filename,txt_suffix) != NULL) ){
                 files_ptr[file_num].file_id = file_num;
                 files_ptr[file_num].file_name = filename;
                 file_num++;
            }

    }
    closedir(dr);

}

/**
 * description: Asks user for index of file choice. returns String of filename
 * params:      files_ptr, num_files
 * returns:     files_ptr[selection_choice].file_name.
 **/
char *get_file_choice(File *files_ptr, int num_files){

    printf("\n===============================\n\tSelect Input File by entering index num : \n");

    for(int index = 0; index < num_files; index++){
        printf("\n\tindex %d | %s", index , files_ptr[index].file_name);
    }

    printf("\n\n\t$ : ");

    int selection_choice = -1;

    while(selection_choice < 0){
        scanf("%d", &selection_choice);
    }

    printf("\n\nSelected index %d : %s", selection_choice, files_ptr[selection_choice].file_name);
    printf("\n===============================");
    return files_ptr[selection_choice].file_name;
}

/**
 * description: Asks user whether or not to continue
 *              running the programme.
 * params:      prg_count
 * returns:     run_prg.
 **/
bool run_prg(int prg_count){
    if(prg_count == 0){
		return true;
	} else {
		char input_char;
		printf("======================================\n\tContunue to run programme? [y/n]");
        printf("\n\n\t$ : ");
		scanf("%s", &input_char);
		return (input_char == 'y') ? true : false;
	}
}


/**
 * description: Reads a file and returns its' contents
 *              as a string.
 * params:      *fileName.
 * returns:     contents.
 **/
char *read_file(char *file_name){
    FILE *file = fopen(file_name, "r");
    char *contents;
    size_t n = 0;
    int c;

    if (file == NULL){
		printf("\n===================\nERROR COULD NOT OPEN FILE\n\n");
		fflush( stdout ); //flush on error
        return NULL; //could not open file
	}

    contents = malloc(MAX_FILE_SIZE);

    while ((c = fgetc(file)) != EOF)    {
        contents[n++] = (char) c;
    }

    // terminate with the null character
    contents[n] = '\0';

	printf("\n===================\nFile Read\n");
	puts(contents);
	printf("\n===================\n");
	fclose(file);
    return contents;
}

/**
 * description: Creates an array of Process structs for
 *              a given string of text from an input file.
 * params:      *file_text, *processes_ptr.
 * returns:     void.
 **/
void create_processes(char *file_text, Process *processes_ptr){

	char *lines;
	char *cells;
	int proc = SENTINEL_VALUE; /**delta below 0 is number of assumed lines for headings**/
	int cell = 0;  /** Assuming Format for each cell : 0 = pid, 1 = at, 2 = bt**/


	/** NOTE* - strtok_r is thread safe (nested use of strtok causes overwrite of static var in method)**/
	while((lines = strtok_r(file_text, "\n", &file_text))) { // iterate through lines delimited by \n
		cell = 0;


		while ((cells = strtok_r(lines, "\t ,\n", &lines))){ // iterate through cells delimeted by \t
			int local_val = atoi(cells);

            if(isdigit(cells[0]) && (proc < 0)){
                proc = 0;
            }

			if(proc >= 0){
				switch(cell){
					case 0:
						processes_ptr[proc].proc_id = local_val;
						break;

					case 1:
						processes_ptr[proc].arrival_time = local_val;
						break;

					case 2:
						processes_ptr[proc].burst_time = local_val;
						processes_ptr[proc].remaining_time = processes_ptr[proc].burst_time;
						break;
				}
			}

			cell++;
		}

		proc++;
	}

}

/**
 * description: Calculates the number of processes within
 *              a read file.
 * params:      *file_text
 * returns:     num_procs.
 **/
int get_num_processes(char *file_text){

	int num_procs = 0;

    if(isdigit(file_text[0])){
        num_procs++;
    }

	for (int i = 0; file_text[i] != 0; i++){
			if((file_text[i] == '\n') && (isdigit(file_text[i+1]))){
				num_procs++;
			}
	}

	printf("\nPROCS : [%d]", num_procs);
	return num_procs;
}

/**
 * description: Prints an output to the stdout for
 *              an array of processes created
 * params:      *processes_ptr, num_processes.
 * returns:     void.
 **/
void print_created_processes_output(Process *processes_ptr, int num_processes){
	printf("\nProcesses Created!\n");
	printf("\nPID\t|AT\t|BT\t|RT\t\n");
	Process this_process;
	for(int index = 0; index < num_processes; index ++){
		this_process = *(processes_ptr + index);
		printf("\n%d\t|%d\t|%d\t|%d\n",this_process.proc_id, this_process.arrival_time, this_process.burst_time, this_process.remaining_time);
	}

}

/**
 * description: Prints a menu of algorithm choices and listens for
 *              user input on stdin. returns user input.
 * params:
 * returns:     choice.
 **/
int algorithm_menu(){
	int choice;
	printf("\nChoose Algorithm:\n");
	printf("\n\t1 | First Come First Served\n");
	printf("\n\t2 | Shortest Job First\n");
	printf("\n\t3 | Round Robin\n");
    printf("\n\n\t$ : ");
	scanf("%d", &choice);
	return choice;

}

/**
 * description: Calculates the sum of all burst times
 *              for a given array of processes.
 * params:      *processes_ptr, num_processes.
 * returns:     sum_of_burst_times.
 **/
int get_sum_of_burst_times(Process *processes_ptr, int num_processes){

	int sum_of_burst_times = 0;

	Process this_process;
	for(int index = 0; index < num_processes; index ++){
		this_process = *(processes_ptr + index);
		sum_of_burst_times += this_process.burst_time;
	}

	printf("\nSum Of Burst Times : %d\n", sum_of_burst_times);

	return sum_of_burst_times;
}

//*****************************************************************************************************
/***********************************************************************
 * Scheduling Algorithms
 *
 * These functions are the algorithms for scheduling.
 * Contain logic for iterating through processes and
 * prioritising / running for time based on algorithm
 * definition.
 ************************************************/


/*******
 * First Come First Served
 *
 * Author:          Nathan Ormond
 * Purpose:         The purpose of this function is to schedule processes
 *                  on a first come first served basis.
 * Explanation:     Non-Preemptive.
 *                  This function iterates for time through an array of
 *                  processes. processes are selected by arrival time and
 *                  then take priority until completed (burst time). the
 *                  completion time will be set for the process and the function
 *                  will iterate to the next lowest arrival time process. repeating
 *                  until the end of the array of processes.
 * Parameters:      *process_ptr, num_processes.
 * Returns:         void. (nothing)
 * Advantages:      No Complex Logic
 *                  Easy to implement
 *                  Eventually every process gets chance to run (no starvation).
 * Disadvantages:   No Pre-emption
 *                  Processes at back of queue may have to wait a long time before being executed
 */
void first_come_first_served(Process *processes_ptr, int num_processes){
	printf("======================================\nFirst Come First Served:\n");
	int sum_of_burst_times = get_sum_of_burst_times(processes_ptr, num_processes);

	int time;
    int working_process_index;
    int index;


    for(time = 1; time <= sum_of_burst_times; time++){
        printf("\n\n========== clock time [%d]\n", time);

        /** Calculating which process to do work with **/
        working_process_index = SENTINEL_VALUE; // sentinel value
		for(index = 0; index < num_processes; index++){
            if((processes_ptr[index].remaining_time != 0) && (processes_ptr[index].arrival_time < time)){  // check process is not invalid by definition (not a four sided triangle)
               if( (working_process_index == SENTINEL_VALUE) || (processes_ptr[working_process_index].arrival_time > processes_ptr[index].arrival_time) ){
                    working_process_index = index;
                }
            }
		}

		printf("set process index (lowest arrival time found): %d\n",working_process_index);
		printf("remaining time : %d\n", processes_ptr[working_process_index].remaining_time);

        if(working_process_index != SENTINEL_VALUE){
            time +=  processes_ptr[working_process_index].remaining_time - 1;
			printf("doing process work... [%d]s \ntime updated : %d\n",processes_ptr[working_process_index].remaining_time, time);
			processes_ptr[working_process_index].remaining_time = 0;
			processes_ptr[working_process_index].completion_time = time;
			processes_ptr[working_process_index].turn_around_time = processes_ptr[working_process_index].completion_time - processes_ptr[working_process_index].arrival_time;
            processes_ptr[working_process_index].wait_time = processes_ptr[working_process_index].turn_around_time - processes_ptr[working_process_index].burst_time;

        }

    }

 }


/*******
 * Shortest Job First
 *
 * Author:           Nathan Ormond
 * Purpose:          The purpose of this function is to schedule processes
 *                   on a shortest job first basis.
 * Explanation:      Non-Preemptive
 *                   Iterates through list of processes, finds the arrived
 *                   process with the lowest burst time and runs it to completion.
 * Parameters:       *processes_ptr, num_processes.
 * Returns:          void. (nothing)
* Advantages:      Short processes executed first.
 *                  output increased as more processes executed in shorter time.
 * Disadvantages:   Time taken by a process must be known by CPU beforehand (not possible in real scheduler)
 *                  Longer processes will have more time waiting (eventually suffer starvation).
 *                  If short processes keep arriving before long process it will never be reached
 */
void shortest_job_first(Process *processes_ptr, int num_processes){

	printf("======================================\nShortest Job First:\n");

	int sum_of_burst_times = get_sum_of_burst_times(processes_ptr, num_processes);

	int time;
    int working_process_index;
    int index;

    for(time = 1; time <= sum_of_burst_times; time++){

        printf("\n\n========== clock time [%d]\n", time);

		/** Calculating which process to do work with **/
        working_process_index = SENTINEL_VALUE; // sentinel value
		for(index = 0; index < num_processes; index++){
            if((processes_ptr[index].remaining_time != 0) && (processes_ptr[index].arrival_time < time)){  // check process is not invalid by definition (not a four sided triangle)
               if( (working_process_index == SENTINEL_VALUE) || (processes_ptr[working_process_index].burst_time > processes_ptr[index].burst_time) ){
                    working_process_index = index;
                }
            }
		}

		printf("set process index (lowest value found): %d\n",working_process_index);
		printf("remaining time : %d\n", processes_ptr[working_process_index].remaining_time);

        /** Logic for performing processing work **/
        if(working_process_index != SENTINEL_VALUE){
			time +=  processes_ptr[working_process_index].remaining_time - 1;
			printf("doing process work... [%d]s \ntime updated : %d\n",processes_ptr[working_process_index].remaining_time, time);
			processes_ptr[working_process_index].remaining_time = 0;
			processes_ptr[working_process_index].completion_time = time;
			processes_ptr[working_process_index].turn_around_time = processes_ptr[working_process_index].completion_time - processes_ptr[working_process_index].arrival_time;
            processes_ptr[working_process_index].wait_time = processes_ptr[working_process_index].turn_around_time - processes_ptr[working_process_index].burst_time;
		}

	}


}

/*******
 * Round Robin
 *
 * Author:          Nathan Ormond
 * Purpose:         The purpose of this function is to schedule processes
 *                  on a round robin basis.
 * Explanation:     Preemptive.
 *                  Processes are added to the queue on a first in first out basis.
 *                  The process at the front of the queue will run for a fixed time quantum.
 *                  After execution for this time quantum the process will be added to the
 *                  back of the queue and the next process will take precedence.
 *                  This will continue until all processes have ran for their burst time.
 * Parameters:      process_ptr, num_processes
 * Returns:         void. (nothing)
 * Advantages:      All processes given the same time priority
 *                  Starvation doesnt occur as each cycle every process is given a fixed time to execute (no process left behind).
 * Disadvantages:   Output depends largely on time quantum.
 *                  if time quantum is longer than needed essentially becomes FCFS with complex logic
 *                  If time quantum is shorter than needed CPU process switches increase.
 *                  Context switching is resource intensive.
 **/
void round_robin(Process *processes_ptr, int num_processes){

    int tq = tq_menu();
    int sum_of_burst_times = get_sum_of_burst_times(processes_ptr, num_processes);
	int time;
    int working_process_index;
    int index;
    int proc_pid_queue[num_processes];

    /**Populates process queue with sentinel value**/
    for(index = 0; index < num_processes; index++){
        proc_pid_queue[index] = SENTINEL_VALUE;
    }

    bool was_last_clock_doing_work = false;
    int last_working_index;

    /**Process scheduling loop**/
    for(time = 1; time <= sum_of_burst_times; time++){
        printf("\n\n========== clock time [%d]\n", time);

        /** Add Processes to Queue if valid **/
        working_process_index = SENTINEL_VALUE;
        for(index = 0; index < num_processes; index++){

            if((processes_ptr[index].remaining_time != 0) && (processes_ptr[index].arrival_time < time)){  // check process is not invalid ( a four sided triangle by definition cannot exist)

                if(working_process_index == SENTINEL_VALUE){
                    working_process_index = index;
                }

                bool is_index_in_queue = false;
                for(int inner_index = 0; inner_index < num_processes; inner_index++){                       // find if index exists in queue
                    if(processes_ptr[index].proc_id == proc_pid_queue[inner_index]){
                        is_index_in_queue = true;
                        break;
                    }
                }

                if(!is_index_in_queue){                                                                     // If index isnt in queue - add to queue at next available slot
                    for(int inner_index = 0; inner_index < num_processes; inner_index++){
                        if(proc_pid_queue[inner_index] == SENTINEL_VALUE){
                            proc_pid_queue[inner_index] = processes_ptr[index].proc_id;
                            break;
                        }
                    }

                }

            }

            if((processes_ptr[index].remaining_time == 0)){                                                // If remaining time == 0 then set to sentinel value
                // find in process queue if exists and set to SENTINEL VALUE
                for(int inner_index = 0; inner_index < num_processes; inner_index++){
                    if(proc_pid_queue[inner_index] == processes_ptr[index].proc_id){
                        proc_pid_queue[inner_index] = SENTINEL_VALUE;
                        break;
                    }
                }

            }

        }

        /** Order Process Queue **/
        if(was_last_clock_doing_work){
            if(proc_pid_queue[0] == processes_ptr[last_working_index].proc_id){
                //shift to back of queue and shift everything else along
                int index_decr = 0;
                for(int index = 1; index < num_processes; index++){
                    proc_pid_queue[index_decr] = proc_pid_queue[index];
                    index_decr ++;
                }

                proc_pid_queue[index_decr] = processes_ptr[last_working_index].proc_id;

            }
        }

        /** Format proc_pid_queue (format out SENTINEL_VALUE) **/
        int *proc_pid_queue_ptr = proc_pid_queue;
        while(!sentinel_values_formatted(proc_pid_queue_ptr, num_processes)){
            for(int index = 0; index < num_processes; index++){
                if(proc_pid_queue[index] == SENTINEL_VALUE){
                    int inner_index_decr = index;
                    for(int inner_index = index + 1; inner_index < num_processes; inner_index++){
                        proc_pid_queue[inner_index_decr] = proc_pid_queue[inner_index];
                        inner_index_decr++;
                    }
                    proc_pid_queue[inner_index_decr] = SENTINEL_VALUE;
                }
            }
        }

        /** Set Working Process Index & print process queue to console**/
        printf("\nPROC QUEUE [ ");
        for(index = 0; index < num_processes; index++){

            if(proc_pid_queue[index] != SENTINEL_VALUE){ //Print process in queue if it is not sentinel value
                printf("PID [%d]", proc_pid_queue[index]);

            } else {                                     //Else print none for sentinel value
                printf("NONE");
            }

            if( (index + 1) != num_processes){          //Seperate by comma (except for last item in array)
                printf(" , ");
            }

            if(processes_ptr[index].proc_id == proc_pid_queue[0]){  // Set working process to first item in array
                working_process_index = index;
            }

        }
        printf(" ]\n\n");

        /** Do Work ...**/
        was_last_clock_doing_work = false; // default
        if(working_process_index != SENTINEL_VALUE){
            printf("Running PID [%d]\nRemaining Time: [%d]\n", processes_ptr[working_process_index].proc_id, processes_ptr[working_process_index].remaining_time);
            if(processes_ptr[working_process_index].remaining_time <= tq){
                // run for remaining time
                time +=  processes_ptr[working_process_index].remaining_time - 1;
                printf("doing process work... [%d]s \n",processes_ptr[working_process_index].remaining_time);
                printf("\nPROCESS COMPLETED [%d]!\n",processes_ptr[working_process_index].proc_id);
                processes_ptr[working_process_index].remaining_time = 0;
                processes_ptr[working_process_index].completion_time = time;
                processes_ptr[working_process_index].turn_around_time = processes_ptr[working_process_index].completion_time - processes_ptr[working_process_index].arrival_time;
                processes_ptr[working_process_index].wait_time = processes_ptr[working_process_index].turn_around_time - processes_ptr[working_process_index].burst_time;
            } else {
                // run for tq
                time +=  tq - 1;
                printf("doing process work... [%d]s \n",tq);
                processes_ptr[working_process_index].remaining_time =  processes_ptr[working_process_index].remaining_time - tq;
            }
            was_last_clock_doing_work = true;
            last_working_index = working_process_index;
        }
    }

    printf("\nRound Robin Completed!\n");
}

/**
 * Validates whether process queue has been formatteed correctly
 * (removing sentinel values between valid PIDs).
 **/
bool sentinel_values_formatted(int *proc_pid_queue_ptr, int num_processes){
    int index_decr = 0;
    for(int index = 1; index < num_processes; index++){
        if((proc_pid_queue_ptr[index_decr] == SENTINEL_VALUE) && (proc_pid_queue_ptr[index] != SENTINEL_VALUE)){
            return false;
        }

        if( (proc_pid_queue_ptr[index_decr] != SENTINEL_VALUE) && (proc_pid_queue_ptr[index] == SENTINEL_VALUE)){  // If valid proc is followed by sentinel value.
            for(int inner_index = index + 1; inner_index < num_processes; inner_index++){
                if(proc_pid_queue_ptr[inner_index] != SENTINEL_VALUE){                                                // If any following value isnt the sentinel value.
                    return false;
                }
            }
        }
        index_decr++;
    }

    return true;
}

/**
 * Menu selection for time quantum to use (round robin algorithm)
 **/
int tq_menu(){
    int choice;
	printf("\n\tEnter digit (1-9) for TQ:\n");
    printf("\n\n\t$ : ");
	scanf("%d", &choice);
	return choice;
}

//*****************************************************************************************************
 /***********************************************************************
 * Outcome Printing Functions
 *
 * These functions calculate and process metrics for processes.
 * Also pretty print functions for displaying metrics to user.
 ************************************************/

/**
 * Prints an outcome table for (populated) *processes_ptr
 * performs pretty print (delimeted by tab), with headings.
 **/
void print_outcome_table(Process *processes_ptr, int num_processes){
	printf("======================================\nOutcomes:\n");
	printf("\nPID\t|  AT\t|  BT\t|  CT\t|  TAT\t|  WT\t|\n");

	Process this_process;
	for(int index = 0; index < num_processes; index++){
		this_process = *(processes_ptr + index);
		printf("\n");
		printf("%d\t|",this_process.proc_id);
		printf("  %d\t|",this_process.arrival_time);
		printf("  %d\t|",this_process.burst_time);
		printf("  %d\t|",this_process.completion_time);
		printf("  %d\t|",this_process.turn_around_time);
		printf("  %d\t|",this_process.wait_time);
		printf("\n");
	}

}

/**
 * Prints the average values for TAT and WT
 * based on contents of *processes_ptr
 **/
void print_averages(Process *processes_ptr, int num_processes){
	printf("======================================\nAverages:\n");
	int sum_tat = 0, sum_wt = 0;

	Process this_process;
	for(int index = 0; index < num_processes; index++){
		this_process = *(processes_ptr + index);
		sum_tat += this_process.turn_around_time;
		sum_wt  += this_process.wait_time;
	}

	float ave_tat = sum_tat / num_processes;
	float ave_wt = sum_wt / num_processes;

	printf("\nAverage TAT = %.3f\n",ave_tat);
	printf("\nAverage WT  = %.3f\n",ave_wt);
}
//*****************************************************************************************************
