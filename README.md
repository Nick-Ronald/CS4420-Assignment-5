# Report

# Compilation Instructions
In order to compile this program, use the following command:  

`g++ cpu_scheduler.cc`  

In addition, you may change the name of the executable to something other than a.out by
adding the following flag to the above compilation command:  

`-o <new_executable_name>`  

Once compiled, in order to run the program, you must use the following structure  

`<executable> <input_file> [FCFS|SJF|RR] <quantum_time>`

Notes:  
-  The input file is designed to be a text file, with the following information on a new line for each process
  -  `<process_id> <arrival_time> <burst_time>`
  -  All fields must be numeric integers
-  The scheduling algorithm must be one of the three listed (not case sensitive)
  -  For Round Robin (RR), the quantum time is required. Otherwise, no need to include it

# Test Cases 

**Test Case 1:**  
PID	Arrival (ms)	Burst (ms)  
1	  0	            3  
2	  1	            5  
3	  2	            6  
4	  3	            1  
5   4             4  
RR Quantum Time: 2  

This test case is the set of processes pulled directly from our previous homework, making it a good starting point to ensure the basic functionality of the program  

Results:  
<img width="1552" height="558" alt="image" src="https://github.com/user-attachments/assets/2d446a48-0b94-4922-8546-89cec47d971a" />
<img width="1642" height="1084" alt="image" src="https://github.com/user-attachments/assets/075491ee-276f-4777-af18-71b0ded2f1e2" />

**Test Case 2:**  
PID	Arrival (ms)	Burst (ms)  
1	  0	            4  
2	  15	          3   
3	  16	          2  
4	  30	          1   
RR Quantum Time: 2  

This test case involves a lot of idle time where processes are around, but they have not arrived yet. This helps simulate how the scheduling algorithm acts during idle time and proves that it does not break because it happened to run into idle time  

Results:  
<img width="1600" height="1200" alt="image" src="https://github.com/user-attachments/assets/c22bc843-7106-4b0d-b6e5-2590236bc305" />


**Test Case 3:**  
PID	Arrival (ms)	Burst (ms)  
5	  0	            8  
1	  0	            4  
4	  10	          4  
3	  10	          2  
2	  0	            2  
RR Quantum Time: 3  

This test case has multiple processes arriving at the same time and also fundamentally reorders the way processes are loaded. In the end, without any priority or difference in arrival time, the first processes loaded will take precedence. It ensures that the simulator works (especially in regards to SJF and RR) when multiple processes arrive at the same time  

Results:  
<img width="1598" height="552" alt="image" src="https://github.com/user-attachments/assets/15dad081-8d7f-4ffe-ae18-fcc5999c84eb" />
<img width="1672" height="958" alt="image" src="https://github.com/user-attachments/assets/9bf6b977-2e47-4b28-b989-f7cfebbfe7c0" />


