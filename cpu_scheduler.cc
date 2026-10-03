/**
* @file: cpu_scheduler.cc
* @author: Nick Ronald
* @date: 09/30/2026
* @brief: This code takes in an input file giving processes, their arrival time, and their burst time, and performs
*         the selected cpu scheduling algorithm on them, and outputs a table with the start time, end time, waiting time,
*         and running time along with the average waiting time
*/  

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <queue>
#include <vector>

using namespace std;

// A structure for a singular process and its relevant information
struct process
{
    int process_id;
    int arrival_time;
    int burst_time;

    void print()
    {
        cout << process_id << " ";
        cout << arrival_time << " ";
        cout << burst_time << endl;
    }
};

// A structure for comparing the burst time of two processes, used for SJF scheduling
struct shorter
{
    bool operator()(const process& p1, const process& p2)
    {
        return p1.burst_time > p2.burst_time;
    }
};

// A structure for a singular burst run of a process and its relevant information
struct run_info
{
    int process_id;
    int arrival_time;
    int start_time;
    int end_time;
    int running_time;
    int waiting_time;
};
/**
* @brief - Reads the input file and creates a vector of processes
*
* @param file_name - The name of the input file
* @return - A vector of processes
*/
vector<process> read_input(string file_name);

/**
* @brief - Performs First-Come, First-Served scheduling
*
* @param processes - A vector of processes
* @return - A vector of run_info structures
*/
vector<run_info> FCFS(vector<process> processes);
/**
* @brief - Performs Shortest Job First scheduling
*
* @param processes - A vector of processes
* @return - A vector of run_info structures
*/
vector<run_info> SJF(vector<process> processes);
/**
* @brief - Performs Round Robin scheduling
*
* @param processes - A vector of processes
* @param qt - The quantum time
* @return - A vector of run_info structures
*/
vector<run_info> RR(vector<process> processes, int qt);
/**
* @brief - Compiles the results of Round Robin scheduling
*
* @param processes - A vector of processes
* @param bursts - A vector of run_info structures
* @return - A vector of run_info structures
*/
vector<run_info> RR_compiled(vector<process> processes, vector<run_info> bursts);
/**
* @brief - Prints the results of the scheduling algorithms
*
* @param results - A vector of run_info structures
* @param compiled_results - A vector of compiled run_info structures
*/
void print_results(vector<run_info> results, vector<run_info> compiled_results);

int main(int argc, char** argv)
{
    if(argc < 2) //Checks if enough input arguments were given
    {
        cout << "Not enough input arguments, please give a scheduling process" << endl;
        exit(1);
    }

    vector<process> processes;
    vector<run_info> results, compiled_results;
    string file_name = argv[1], schedule = argv[2];
    int quantum_time;

    for(size_t i = 0; i < schedule.size(); i++) //Allows for case-insensitive input of scheduling algorithm
    {
        schedule.at(i) = toupper(schedule.at(i));
    }

    if(schedule != "FCFS" && schedule != "RR" && schedule != "SJF") //Checks if a valid scheduling algorithm was entered
    {
        cout << "A valid scheduling algorithm was not entered." << endl;
        exit(1);
    }

    if(argc == 4 && schedule == "RR") // Checks if a quantum time was entered for Round Robin scheduling
    {
        quantum_time = stoi(argv[3]);
    }
    else if(schedule == "RR")
    {
        cout << "For Round Robin scheduling, a quantum time must also be entered." << endl;
        exit(1);
    }

    processes = read_input(file_name); //Reads the input file and creates a vector of processes

    //Performs the selected scheduling algorithm and prints the results
    if(schedule == "FCFS")
    {
        results = FCFS(processes);
        print_results(results, {});
    }
    else if(schedule == "SJF")
    {
        results = SJF(processes);
        print_results(results, {});
    }
    else if(schedule == "RR")
    {
        results = RR(processes, quantum_time);
        compiled_results = RR_compiled(processes, results);
        print_results(results, compiled_results);
    }
}

vector<process> read_input(string file_name)
{
    ifstream input(file_name);

    vector<process> processes;
    process new_process;
    int count = 0;
    while(!input.eof())
    {
        input >> new_process.process_id;
        input >> new_process.arrival_time;
        input >> new_process.burst_time;

        processes.push_back(new_process);
    }   
    input.close();
    return processes;
}

vector<run_info> FCFS(vector<process> processes)
{
    int current_ms = 0, unfinished = processes.size();
    bool change = true; //Start this boolean as true 
    queue<process> ready_queue;
    vector<run_info> bursts;
    process current_process;
    run_info current_burst;

    while(unfinished != 0) // While there is at least 1 process running
    {
        for(size_t i = 0; i < processes.size(); i++) // At each time interval, check if there are new processes to add to the queue
        {
            if(processes.at(i).arrival_time == current_ms)
            {
                ready_queue.push(processes.at(i));
            }
        }
        if(change) //If a new process is starting, start logging a new set of run_info
        {
            if(!ready_queue.empty()) //Ensures that if not all processes have arrived yet, the current process is not set to an empty process
            {
                current_process = ready_queue.front(); //Obtain the current process running
                change = false;
            }
            run_info new_burst; //Log the new process starting its burst
            new_burst.process_id = current_process.process_id;
            new_burst.arrival_time = current_process.arrival_time;
            new_burst.start_time = current_ms;
            new_burst.waiting_time = new_burst.start_time - new_burst.arrival_time;
            current_burst = new_burst;
            //cout << "Process " << current_process.process_id << " has started at time " << current_ms << endl;
        }

        current_process.burst_time = current_process.burst_time - 1; //Decrement burst
        if(current_process.burst_time == 0) //If the process has completed
        {
            ready_queue.pop(); //Remove it from the front of the ready queue
            unfinished--; //Decrement the count of unfinished processes
            current_burst.end_time = current_ms + 1; //Log the end time of the burst
            current_burst.running_time = current_burst.end_time - current_burst.start_time;
            bursts.push_back(current_burst); //Add the completed burst to the vector of bursts
            change = true;
        }

        current_ms++; //Advance time
    }

    return bursts;
}

vector<run_info> SJF(vector<process> processes)
{

    int current_ms = 0, unfinished = processes.size();
    bool change = true; //Start this boolean as true 
    priority_queue<process, vector<process>, shorter> ready_queue;
    vector<run_info> bursts;
    process current_process;
    run_info current_burst;

    while(unfinished != 0) // While there is at least 1 process running
    {
        for(size_t i = 0; i < processes.size(); i++) // At each time interval, check if there is are new processes to add to the queue
        {
            if(processes.at(i).arrival_time == current_ms) //If a new process is arriving, add it to the ready queue
            {
                ready_queue.push(processes.at(i));
            }
        }

        if(change) //If a new process is starting, start logging a new set of run_info
        {
            if(!ready_queue.empty()) //Ensures that if not all processes have arrived yet, the current process is not set to an empty process
            {
                current_process = ready_queue.top(); //Obtain the current process running
                ready_queue.pop(); //Remove it from the ready queue once the process has been selected to run
                change = false;
            }
            run_info new_burst; //Log the new process starting its burst
            new_burst.process_id = current_process.process_id;
            new_burst.arrival_time = current_process.arrival_time;
            new_burst.start_time = current_ms;
            new_burst.waiting_time = new_burst.start_time - new_burst.arrival_time;
            current_burst = new_burst;
        }

        current_process.burst_time = current_process.burst_time - 1; //Decrement burst
        if(current_process.burst_time == 0) //If the process has completed
        {
            unfinished--; //Decrement the count of unfinished processes
            current_burst.end_time = current_ms + 1; //Log the end time of the burst
            current_burst.running_time = current_burst.end_time - current_burst.start_time;
            bursts.push_back(current_burst); //Add the completed burst to the vector of bursts
            change = true;
        }

        current_ms++; //Advance time
    }

    return bursts;
}

vector<run_info> RR(vector<process> processes, int qt)
{
    int current_ms = 0, unfinished = processes.size(), qt_end = qt;
    bool change = true; //Start this boolean as true 
    queue<process> ready_queue;
    vector<run_info> bursts;
    process current_process;
    run_info current_burst;

    while(unfinished != 0) // While there is at least 1 process running
    {
        for(size_t i = 0; i < processes.size(); i++) // At each time interval, check if there are new processes to add to the queue
        {
            if(processes.at(i).arrival_time == current_ms)
            {
                ready_queue.push(processes.at(i));
            }
        }

        if(current_ms == qt_end && !change && !ready_queue.empty()) //If the quantum time has been reached, and a new process is not already starting, change to the next process
        {
            ready_queue.pop(); //Remove it from the front of the ready queue
            current_burst.end_time = current_ms; //Log the end time of the burst, not +1 as this active time IS the end time
            current_burst.running_time = current_burst.end_time - current_burst.start_time;
            bursts.push_back(current_burst); //Add the completed burst to the vector of bursts
            ready_queue.push(current_process); //Add the current process back to the ready queue
            change = true;
        }

        if(change) //If a new process is starting, start logging a new set of run_info
        {
            if(!ready_queue.empty()) //Ensures that if not all processes have arrived yet, the current process is not set to an empty process
            {
                current_process = ready_queue.front(); //Obtain the current process running
                change = false;
            }
            run_info new_burst; //Log the new process starting its burst
            new_burst.process_id = current_process.process_id;
            new_burst.arrival_time = current_process.arrival_time;
            new_burst.start_time = current_ms;
            new_burst.waiting_time = 0; //Waiting time is not calculated right now as it may not be accurate for RR
            current_burst = new_burst;
            qt_end = current_ms + qt; //Set the end of the quantum time for this process
     }

        current_process.burst_time = current_process.burst_time - 1; //Decrement burst
        if(current_process.burst_time == 0) //If the process has completed
        {
            ready_queue.pop(); //Remove it from the front of the ready queue
            unfinished--; //Decrement the count of unfinished processes
            current_burst.end_time = current_ms + 1; //Log the end time of the burst
            current_burst.running_time = current_burst.end_time - current_burst.start_time;
            bursts.push_back(current_burst); //Add the completed burst to the vector of bursts
            change = true;
        }

        current_ms++; //Advance time
    }

    return bursts;
}

vector<run_info> RR_compiled(vector<process> processes, vector<run_info> bursts)
{
    bool first_burst_seen, finished;
    vector<run_info> results;
    for(size_t i = 0; i < processes.size(); i++) //For each process, compile the results of all of its bursts (primarily for waiting time)
    {
        run_info current_process; //Prepare the current process's compiled run_info
        first_burst_seen = false; finished = false;
        current_process.process_id = processes.at(i).process_id;
        current_process.arrival_time = processes.at(i).arrival_time;
        current_process.running_time = 0; //Starts at 0 to add up over time (allows checking for when the process is finished)
        for(size_t j = 0; j < bursts.size(); j++) //For each process, check all of the bursts to see if they belong to it, and if so, compile the results
        {
           if(bursts.at(j).process_id == processes.at(i).process_id)
           {
                if(!first_burst_seen) //Track whether the first burst has been seen for initial waiting time calculation
                {   
                    first_burst_seen = true;
                    current_process.start_time = bursts.at(j).start_time;
                    current_process.waiting_time = current_process.start_time - current_process.arrival_time; //Initial waiting time is added to the current process
                }
                current_process.running_time += bursts.at(j).running_time; //The running time is updated to include the most recent burst running time
                current_process.end_time = bursts.at(j).end_time; //The end time is updated to the most recent burst end time
                if(current_process.running_time == processes.at(i).burst_time) //If the running time is equal to the total burst time, the process has finished
                {
                    finished = true;
                }
           }
           else
           {
                if(first_burst_seen && !finished) //If the first burst has been seen, and the current process has not finished, add the time spent running other processes to the waiting time for this one
                {
                    current_process.waiting_time += bursts.at(j).running_time;
                }
           }
            
        }
        results.push_back(current_process);
    }
    return results;
}

void print_results(vector<run_info> results, vector<run_info> compiled_results)
{
    if(compiled_results.size() == 0) //If the compiled results are empty, print the results of the bursts
    {
        cout << "Process ID\tArrival Time\tStart Time\tEnd Time\tRunning Time\tWaiting Time" << endl;
        cout << setfill('-') << setw(100) << "-" << setfill(' ') << endl;
        for(size_t i = 0; i < results.size(); i++)
        {
            cout << results.at(i).process_id << "\t\t" << results.at(i).arrival_time << "\t\t" << results.at(i).start_time
            << "\t\t" << results.at(i).end_time << "\t\t" << results.at(i).running_time << "\t\t" << results.at(i).waiting_time << endl;
        }
    }
    else //If the compiled results are not empty, print the burst results and compiled results
    {
        cout << "Process ID\tStart Time\tEnd Time\tRunning Time\t" << endl;
        cout << setfill('-') << setw(100) << "-" << setfill(' ') << endl;
        for(size_t i = 0; i < results.size(); i++)
        {
            cout << results.at(i).process_id << "\t\t" << results.at(i).start_time << "\t\t" << 
            results.at(i).end_time << "\t\t" << results.at(i).running_time << "\t\t" << endl << endl;
        }

        cout << "Process ID\tArrival Time\tEnd Time\tRunning Time\tWaiting Time" << endl;
        cout << setfill('-') << setw(100) << "-" << setfill(' ') << endl;
        for(size_t i = 0; i < compiled_results.size(); i++)
        {
            cout << compiled_results.at(i).process_id << "\t\t" << compiled_results.at(i).arrival_time << "\t\t" << compiled_results.at(i).end_time << "\t\t" 
            << compiled_results.at(i).running_time << "\t\t" << compiled_results.at(i).waiting_time << endl;
        }
    }
}
