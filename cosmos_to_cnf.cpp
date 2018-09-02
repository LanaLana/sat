#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int num_satellites, num_tasks, time_interval;
int total_duration = 0;
int num_clauses = 0;
vector<vector<int>> time_to_tasks; /* for each point in time
contain all tasks started there */

struct Task {
    int index;
    int start_time;
    int finish_time;
    int duration;
    int position;
    int start_var_index;
};

vector<Task> tasks;

// just one task can be handled at the particular point in time by one satellite
void constraint_1() {
    for (int st = 0; st < num_satellites; ++st) {
        for (int t = 0; t < tasks.size(); ++t) {
            for (int d = 0; d < tasks[t].duration; ++d) {
                int time_point = tasks[t].start_time + d;
                for (int tt = 0; tt < time_to_tasks[time_point].size(); ++tt) {
                    int another_t = time_to_tasks[time_point][tt];
                    if (t != another_t) {
                        cout << -(st * total_duration + tasks[t].start_var_index + 1) << " " << -(st * total_duration + tasks[another_t].start_var_index + 1) << endl;
                        num_clauses++;
                    }
                }
            }
        }
    }
    return;
}

// two satellites can't handle the same task 
void constraint_2() {
    for (int i = 0; i < tasks.size(); ++i) {
        for (int st1 = 0; st1 < num_satellites - 1; ++st1) {
            for (int st2 = st1 + 1; st2 < num_satellites; ++st2) {
                cout << -(st1 * total_duration + tasks[i].start_var_index + 1) << " " << -(st2 * total_duration + tasks[i].start_var_index + 1) << endl;                 
                num_clauses++;
            }
        }
    }
    return;
}

// prohibited to change position rapidly
void constraint_3() {
    for (int i = 0; i < tasks.size(); ++i) {
        int finish_time = tasks[i].finish_time;
        for (int t = 0; t < time_to_tasks[finish_time].size(); ++t) {
            int next_task = time_to_tasks[finish_time][t];
            if (tasks[i].position > tasks[next_task].position + 1 || tasks[i].position < tasks[next_task].position - 1) {
                for (int st = 0; st < num_satellites; ++st) {
                    cout << -(total_duration * st + tasks[i].start_var_index + 1) << " " << -(total_duration * st + tasks[next_task].start_var_index + 1) << endl;
                    num_clauses++;
                }
            }
        }
    }
    return;
}

// each task has to be fully completed (if duration is greater than 1)
void constraint_4() {
    for (int st = 0; st < num_satellites; ++st) {
        for (int t = 0; t < tasks.size(); ++t) {
            for (int duration = 1; duration < tasks[t].duration; ++ duration) {
                cout << -(st * total_duration + 1) << " " << st * total_duration + duration + 1 << endl;
                cout << st * total_duration + 1 << " " << -(st * total_duration + duration + 1) << endl;
                num_clauses += 2;
            }
        }
    }    
    return;
}

// all tasks have to be handled
void constraint_5() {
    for (int t = 0; t < tasks.size(); ++t) {
        string str = "";
        for (int st = 0; st < num_satellites; ++st) {
            str.append(to_string(st * total_duration + tasks[t].start_var_index + 1));
            str.append(" ");
        }
        cout << str << endl;
        num_clauses++;
    }
    return;
}

void cosmos_to_sat() {
   // cout << total_duration * num_satellites << endl;
    constraint_1();
    constraint_2();
    constraint_3();
    constraint_4();
    constraint_5();
    cout << total_duration * num_satellites << " " << num_clauses << endl; 
    return;
}

int main() {
    
    ifstream in("input.txt");
    streambuf *cinbuf = std::cin.rdbuf(); //save old buf
    cin.rdbuf(in.rdbuf()); //redirect std::cin to in.txt!

    ofstream out("out.txt");
    streambuf *coutbuf = cout.rdbuf(); // save old buf
    cout.rdbuf(out.rdbuf()); // redirect std::cout to out.txt!

    cin >> num_satellites >> num_tasks >> time_interval;
    
    int var = 1;
    time_to_tasks.resize(time_interval);
    Task task_input;
    for (int i = 0; i < num_tasks; ++i) {
        cin >> task_input.start_time >> task_input.finish_time >> task_input.position;
        task_input.duration = task_input.finish_time - task_input.start_time;
        task_input.start_var_index = total_duration;
        tasks.push_back(task_input);
        total_duration += task_input.duration;
        time_to_tasks[task_input.start_time].push_back(i);
    }
    cosmos_to_sat();
    cin.rdbuf(cinbuf);   
    cout.rdbuf(coutbuf); 
    return 0;
}
