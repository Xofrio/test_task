#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <unistd.h>
#include <sys/types.h>

#define amount_objects_maximum 	100U
#define amount_input 	        4U
#define order                   3U
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

#define fscanf_data( array, o, i, part )   \
fscanf( data, format_input( array[o][i].part ), &array[o][i].part )

#define format_input( T )   \
_Generic                    \
(                           \
    ( T ),                  \
    float:      "%f ",      \
    double:     "%lf ",     \
    default:    "%Lf "      \
)
#define format_output( T )  \
_Generic                    \
(                           \
    ( T ),                  \
    float:      "%f",       \
    double:     "%lf",      \
    default:    "%Lf"       \
)
#define pi_div_180( T )                                 \
_Generic                                                \
(                                                       \
    ( T ),                                              \
    float:      0.0174532925199432957692369076848861F,  \
    double:     0.0174532925199432957692369076848861,   \
    default:    0.0174532925199432957692369076848861L   \
)
#define radius_earth( T )               \
_Generic                                \
(                                       \
    ( T ),                              \
    float:      6371.0087714150004F,    \
    double:     6371.0087714150004,     \
    default:    6371.0087714150004L     \
)
#define to_radians( x ) ( x ) * pi_div_180( x )
#define critical_distance( T )  \
_Generic                        \
(                               \
    ( T ),                      \
    float:      400.F,          \
    double:     400.,           \
    default:    400.L           \
)
#define warning_distance_1( T ) \
_Generic                        \
(                               \
    ( T ),                      \
    float:      2500.F,         \
    double:     2500.,          \
    default:    2500.L          \
)
#define warning_distance_2( T ) \
_Generic                        \
(                               \
    ( T ),                      \
    float:      1600.F,         \
    double:     1600.,          \
    default:    1600.L          \
)
#define warning_velocity( T )   \
_Generic                        \
(                               \
    ( T ),                      \
    float:      90000.F,        \
    double:     90000.,         \
    default:    90000.L         \
)

typedef long double units;

/**
 * @brief Contains latitude, longitude, altitude and time in units
 */
struct data_input
{
    units latitude;
    units longitude;
    units altitude;
    units time;
};

/**
 * @brief Contains distance and approach velocity in units
 */
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
extern units                g_time;
extern size_t               g_amount_objects;
extern size_t               g_amount_observations;
extern bool                 g_write_happened;

#endif // AUXILIARY_H
