#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define amount_objects_maximum 	100U
#define amount_input 	        4U
#define order                   6U
#define amount_seconds_sleep    1U
#define size_file_name          256U
#define amount_files            2U
#define file_index_output       1U
#define file_index_input        0U

#define file_name_input         "input"
#define file_name_output        "output"
#define file_extension          ".json"
#define file_tag( index )           \
index == file_index_input ?         \
file_name_input file_extension :    \
file_name_output file_extension

#define format_input( T ) _Generic  \
(                                   \
    ( T ),                          \
    float:      "%f ",              \
    double:     "%lf ",             \
    default:    "%Lf "              \
)
#define format_output( T ) _Generic \
(                                   \
    ( T ),                          \
    float:      "%f",               \
    double:     "%lf",              \
    default:    "%Lf"               \
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

extern struct data_output   g_output[amount_objects_maximum];
extern char                 g_files[amount_files][size_file_name];
extern FILE *               g_data_output;
extern pthread_cond_t       g_condition;
extern pthread_mutex_t      g_mutex;
extern pthread_t            g_thread_process;
extern pthread_t            g_thread_recommend;
extern units                g_time;
extern size_t               g_amount_objects;
extern bool                 g_write_happened;

#endif // AUXILIARY_H
