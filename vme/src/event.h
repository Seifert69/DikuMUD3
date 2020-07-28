/*
 $Author: tperry $
 $RCSfile: event.h,v $
 $Date: 2002/04/26 00:53:24 $
 $Revision: 2.4 $
 */

#ifndef _MUD_EVENT_H
#define _MUD_EVENT_H
struct eventq_elem
{
    void (*func) (void *, void *);
    int when;
    void *arg1;
    void *arg2;
};

class eventqueue
{
private:
    int count;
    int heapsize;
    int loop_process;
    float loop_time;
    float max_loop_time;
    int max_loop_time_process;
    float max_process_time;
    int max_process;
    int total_loops;
    double total_process;
    float total_time;
    struct eventq_elem **heap;
public:
    eventqueue (void);
    ~eventqueue (void);

    inline int Count (void) {
        return count;
    }
    inline int NextEventTic (void) {
        return (count>0?heap[1]->when:0);
    }
    inline int PCount (void) {
        return loop_process;
    }
    inline float PTime (void) {
        return loop_time;
    }
    inline float Max_PTime (void) {
        return max_loop_time;
    }
    inline int Max_PTime_Process (void) {
        return max_loop_time_process;
    }
    inline int Max_PCount (void) {
        return max_process;
    }
    inline float Max_PCount_Time (void) {
        return max_process_time;
    }
    inline double Total_PCount (void) {
        return total_process;
    }
    inline int Total_Loops (void) {
        return total_loops;
    }
    inline float Total_PTime (void) {
        return total_time;
    }
    inline float Avg_PTime (void) {
        return (float)total_time/(float)total_loops;
    }
    inline float Avg_PCount (void) {
        return
            (float)total_process/(float)total_loops;
    }
    eventq_elem *add (int when, void (*func) (void *, void *),
                      void *arg1, void *arg2);
    void remove (void (*func) (void *, void *), void *arg1, void *arg2);
    void remove_relaxed (void (*func) (void *, void *), void *arg1, void *arg2);
    void process (void);
};

#endif
