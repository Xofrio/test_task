#include "process.h"

/**
 * @brief           Computes distance between two objects.
 * @param object_1  first object
 * @param object_2  second object
 * @return          squared distance between two objects in 3d
 */
static units get_distance
(
    struct coordinates const * const object_1,
    struct coordinates const * const object_2
)
{
    units theta_1           = to_radians( object_1->latitude );
    units theta_2           = to_radians( object_2->latitude );
    units delta_altitude    = object_1->altitude - object_2->altitude;
    units distance_2D       =
    acos
    (
        sin( theta_1 ) * sin( theta_2 )
    +   cos( theta_1 ) * cos( theta_2 )
        * cos( to_radians( object_1->longitude ) - to_radians( object_2->longitude ))
    ) * radius_earth( delta_altitude );

    return distance_2D * distance_2D + delta_altitude * delta_altitude;
}

/**
 * @brief           Makes a guess coordinate based on given coordinates.
 * @param order_2   current observation
 * @param order_1   previous observation
 * @param order_0   previous to previous observation
 * @return          2nd order approximation - coordinate prediction
 */
static units make_guess
(
    units const order_2,
    units const order_1,
    units const order_0
)
{
    return
        two_point_five( order_2 ) * order_2
    -   ( order_1 + order_1 )
    +   point_five( order_0 ) * order_0;
}

/**
 * @brief           Makes a guess for all 3 coordinates.
 * @param object    current object
 * @param order_2   previous observation
 * @param order_1   previous to previous observation
 * @param order_0   previous to previous to previous observation
 * @param mode      true means that we make a guess from 3 real parts,
 *                  false means that we make a guess from 2 real parts
 *                  and 1 guess part
 */
static void guess_coordinates
(
    struct object_data * const  object,
    struct coordinates * const  position,
    size_t  const               order_2,
    size_t  const               order_1,
    size_t  const               order_0,
    bool    const               mode
)
{
    position->latitude =
    guess
    (
        object,
        ( mode ? object->real[order_2] : object->guess ),
        order_1,
        order_0,
        latitude
    );
    position->longitude =
    guess
    (
        object,
        ( mode ? object->real[order_2] : object->guess ),
        order_1,
        order_0,
        longitude
    );
    position->altitude = 
    guess
    (
        object,
        ( mode ? object->real[order_2] : object->guess ),
        order_1,
        order_0,
        altitude
    );
}

/**
 * @brief       Fills input array with data from specified json file.
 * @param input array to store data
 * @param data  json file containing data
 * @param index index of the current object
 */
static void get_data
(
    struct object_data          input[],
    FILE *              const   data,
    size_t              const   index
)
{
    struct coordinates guess_alternative;
    struct coordinates guess_previous;
    size_t const order_2 = ( input[index].observations + order - 1 ) % order;
    size_t const order_1 = ( input[index].observations + order - 2 ) % order;
    size_t const order_0 = input[index].observations % order;

    if ( input[index].observations > 3 )
    {
        guess_coordinates
        (
            &input[index],
            &guess_alternative,
            order_2,
            order_1,
            order_0,
            false
        );
    }

    if ( input[index].observations > 2 )
    {
        guess_previous = input[index].guess.position;
        guess_coordinates
        (
            &input[index],
            &input[index].guess.position,
            order_2,
            order_1,
            order_0,
            true
        );
        input[index].guess.time = input[index].real[order_2].time + 1;
    }

    size_t current_order = input[index].observations++ % order;
    for
    ( 
        size_t j = 0;
        j < amount_input;
        fscanf( data, "%*s" ),
        !j      ? fscanf_data( data, input, current_order, index, position.latitude )
    :   j == 1  ? fscanf_data( data, input, current_order, index, position.longitude )
    :   j == 2  ? fscanf_data( data, input, current_order, index, position.altitude )
                : fscanf_data( data, input, current_order, index, time ),
        fscanf( data, "%*c" ),
        ++j
    );
    input[index].real[current_order].position.altitude *= 
    kilometers_per_meter( input[index].real[current_order].position.altitude );

    if
    (
        input[index].observations > 4
    &&  get_distance
        (
            &input[index].real[order_0].position,
            &input[index].guess.position
        )
        >
        get_distance
        (
            &input[index].real[order_0].position,
            &guess_alternative
        )
    )
    {
        input[index].real[order_2].position = guess_previous;
        input[index].guess.position = guess_alternative;
    }  
}

/**
 * @brief                   Computes approach velocity between two objects.
 * @param first_current     current observation of first object
 * @param first_previous    previous observation of first object
 * @param second_current    current observation of second object
 * @param second_previous   previous observation of second object
 * @return                  squared approach velocity between two objects in 3d
 */
static units get_approach_velocity
(
    struct data_input const * const first_current,
    struct data_input const * const first_previous,
    struct data_input const * const second_current,
    struct data_input const * const second_previous
)
{
    return
    (
        sqrt( get_distance( &first_previous->position, &second_previous->position ) )
    -   sqrt( get_distance( &first_current->position, &second_current->position ) )
    ) / ( second_current->time - second_previous->time ) * seconds_per_hour;
}

/**
 * @brief           Compares id's of two objects of type object_data.
 * @param first     first object
 * @param second    second object
 * @return          1, if first id is higher,
 *                  0, if first id is equal to second,
 *                  -1, if first id is lower
 */
static int object_id_ascending
(
    void const * const first,
    void const * const second
)
{
    size_t first_id     = (( struct object_data * ) first )->id;
    size_t second_id    = (( struct object_data * ) second )->id;

    return first_id > second_id ? 1 : first_id == second_id ? 0 : -1;
}

/**
 * @brief           Swaps two objects of type object_data.
 * @param first     first object
 * @param second    second object
 */
static void swap
(
    struct object_data * first,
    struct object_data * second
)
{
    struct object_data temp = *first;

    *first                  = *second;
    *second                 = temp;
}

/**
 * @brief           Searches for an object in specified range by specified id.
 * @param begin     begin of the range
 * @param end       end of the range (inclusive)
 * @param id        id with wich object should be found
 * @return          index of such an object or
 *                  index greater than maximum, if object does not exist
 */
static size_t find_by_id
(
    struct object_data *    begin,
    struct object_data *    end,
    size_t const            id
)
{
    struct object_data * begin_initial = begin;

    for
    (
        struct object_data * middle;
        begin <= end;
        middle->id < id ? ( begin = middle + 1 ) : ( end = middle - 1 )
    )
    {
        middle = begin + ( end - begin ) / 2;

        if ( middle->id == id ) return middle - begin_initial + objects_start;
    }

    return amount_objects_maximum;
}

void * process()
{
    FILE *  data_input;
    size_t  current;
    size_t  id;
    size_t  i;
    size_t  amount_to_delete;
    size_t  self_order_current;
    size_t  self_order_previous;
    size_t  object_order_current;
    size_t  object_order_previous;
    char    has_other_objects;
    bool    got_new = false;

    for (;;)
    {
        pthread_mutex_lock( &g_mutex );

        amount_to_delete = has_other_objects = 0;

        data_input = fopen( g_input_file, "r" );    // TODO: change to g_files[file_index_input]
        fscanf( data_input, "%*c %*s %*c" );
        get_data( g_data, data_input, 0 );
        g_data[0].id = 0;                           // TODO: might be unnecessary, so delete this
        fscanf( data_input, "%*c %*s %*c %c", &has_other_objects );
        for
        (
            ;
            has_other_objects == '{'
            && g_amount_objects < amount_objects_maximum;
            fscanf( data_input, "%*s %zu,", &id ),
            current =
            find_by_id
            (
                g_data + objects_start,
                g_data + g_amount_objects,
                id
            ),
            (
                current == amount_objects_maximum
                ?
                (
                    current                         = ++g_amount_objects,
                    g_data[current].id              = id,
                    g_data[current].observations    = 0,
                    got_new                         = true
                )
                :   0
            ),
            get_data( g_data, data_input, current ),
            (
                got_new
                ?
                (
                    qsort
                    (
                    g_data,
                    g_amount_objects + objects_start,
                    sizeof( g_data[0] ),
                    object_id_ascending
                    ),
                    got_new = false
                )
                : 0
            ),
            fscanf( data_input, "%*s %c", &has_other_objects )
        );
        fclose( data_input );

        self_order_current                          =
        ( g_data[0].observations + order - 1 ) % order;
        self_order_previous                         =
        ( g_data[0].observations + order - 2 ) % order;
        g_time                                      =
        g_data[0].real[self_order_current].time;

        for
        (
            i = objects_start;
            i < g_amount_objects + objects_start;
            (
                g_data[i].real[( g_data[i].observations + ( order - 1 )) % order].time
            !=  g_time
            ?   ( ++amount_to_delete, g_data[i].id = __SIZE_MAX__ )
            :   0
            ),
            ++i
        );
        qsort
        (
            g_data,
            g_amount_objects + objects_start,
            sizeof( g_data[0] ),
            object_id_ascending
        );
        g_amount_objects -= amount_to_delete;

        for
        (
            i = objects_start;
            i < g_amount_objects + objects_start;
            ++i
        )
        {
            object_order_current                =
            ( g_data[i].observations + order - 1 ) % order;
            object_order_previous               =
            ( g_data[i].observations + order - 2 ) % order;

            g_data[i].distance =
            get_distance
            (
                &g_data[0].real[self_order_current].position,
                &g_data[i].real[object_order_current].position
            );
            if ( g_data[i].observations > 1 )
            {
                g_data[i].approach_velocity =
                get_approach_velocity
                (
                    &g_data[0].real[self_order_current],
                    &g_data[0].real[self_order_previous],
                    &g_data[i].real[object_order_current],
                    &g_data[i].real[object_order_previous]
                );
            }
            else
            {
                g_data[i].approach_velocity = 0;
            }

            if ( g_data[i].observations < 4 )
            {
                g_data[i].approach_velocity_guess = 0;
                g_data[i].distance_guess = warning_distance_1 + warning_distance_1;
            }
            else
            {
                g_data[i].distance_guess =
                get_distance
                (
                    &g_data[0].guess.position,
                    &g_data[i].guess.position
                );
                g_data[i].approach_velocity_guess =
                get_approach_velocity
                (
                    &g_data[0].guess,
                    &g_data[0].real[self_order_previous],
                    &g_data[i].guess,
                    &g_data[i].real[object_order_previous]
                );
            }
        }

        g_input_file[8] = g_amount_observations + 1 + '0';  // TODO: remove this.

        g_write_happened = true;
        pthread_cond_signal( &g_condition );
        pthread_mutex_unlock( &g_mutex );

        sleep( amount_seconds_sleep );
    }
}
