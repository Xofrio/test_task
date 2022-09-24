#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#define amount_objects_maximum 	100U
#define amount_input 	        4U
#define order                   6U
#define amount_seconds_sleep    1U
#define amount_pipe_descriptors 2U
#define pipe_descriptor_read    0U
#define pipe_descriptor_write   1U
#define size_file_name          32U
#define format( T ) _Generic    \
(                               \
    ( T ),                      \
    float:      "%f ",          \
    double:     "%lf ",         \
    default:    "%Lf "          \
)
#define critical_distance( T ) _Generic \
(                                       \
    ( T ),                              \
    float:      20.F,                   \
    double:     20.,                    \
    default:    20.L                    \
)
#define warning_distance_1( T ) _Generic    \
(                                           \
    ( T ),                                  \
    float:      50.F,                       \
    double:     50.,                        \
    default:    50.L                        \
)
#define warning_distance_2( T ) _Generic    \
(                                           \
    ( T ),                                  \
    float:      40.F,                       \
    double:     40.,                        \
    default:    40.L                        \
)
#define warning_velocity( T ) _Generic  \
(                                       \
    ( T ),                              \
    float:      300.F,                  \
    double:     300.,                   \
    default:    300.L                   \
)

typedef long double units;

struct data_input
{
    units latitude;
    units longitude;
    units altitude;
    units time;
};

struct data_output
{
    units distance;
    units approach_velocity;
};

extern struct data_output g_output[amount_objects_maximum];
extern char g_file_input[size_file_name];
extern char g_file_output[size_file_name];
extern pthread_cond_t g_condition;
extern pthread_mutex_t g_mutex;
extern units g_time;
extern size_t g_amount;
extern bool g_write_happened;

#endif // AUXILIARY_H
