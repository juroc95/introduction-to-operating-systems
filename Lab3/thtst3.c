#include "types.h"
//#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "victor_thread.h"

#include "thtst.h"

#ifdef VTHREADS

#ifndef DEFAULT_NUM_THREADS
# define DEFAULT_NUM_THREADS 4
#endif // DEFAULT_NUM_THREADS

// I have run with 10 threads, but not more
#ifndef MAX_THREADS
# define MAX_THREADS 10
#endif // MAX_THREADS

// very slow disk i/o
#define MAX_MATRIX_SIZE 40

#define MYRAND_MAX  (1 << 31)

#ifndef MATRIX_TYPE
# define MATRIX_TYPE int
#endif // MATRIX_TYPE

typedef struct matrix_s {
    uint rows;
    uint cols;
    MATRIX_TYPE **data;
} matrix_t;

static long num_threads = DEFAULT_NUM_THREADS;
static matrix_t *omatrix = NULL;    // the output matrix - goes to default file
static matrix_t *lmatrix = NULL;    // the lefthand matrix
static matrix_t *rmatrix = NULL;    // the righthand matrix

static void gen_data(matrix_t *matrix);
static void free_matrix(matrix_t *matrix);
static void calc_row(matrix_t *lmatrix, matrix_t *rmatrix, matrix_t *omatrix, uint row);
static void output_matrix(matrix_t *matrix, char *fname);

static void cal_rowThreadStep(void *arg);

static const unsigned long DIVISOR = (1 << 16);
static unsigned long next = 1;

// taken from the rand(3) man page.
static unsigned long
myrand(void)
{
    next = next * 1103515245 + 12345;
    
    return((unsigned) (next / DIVISOR) % MYRAND_MAX);
}

static void
mysrand(unsigned seed)
{
    next = seed;
}
#endif // VTHREADS

int
main(int argc, char **argv)
{
#ifdef VTHREADS
    int isVerbose = FALSE;
    int pid = -1;

    // 3 really is a good number
    mysrand(3);

    pid = fork();
    if (0 == pid) {
        lmatrix = malloc(sizeof(matrix_t));
        rmatrix = malloc(sizeof(matrix_t));
        omatrix = malloc(sizeof(matrix_t));

        if (argc > 1) {
            num_threads = atoi(argv[1]);
            if (num_threads < 1 || num_threads > MAX_THREADS) {
                printf(1, "bad thread count - using %d\n", DEFAULT_NUM_THREADS);
                num_threads = DEFAULT_NUM_THREADS;
            }
        }
        printf(1, "num threads %d\n", num_threads);
        if (num_threads < 1 || num_threads > MAX_THREADS) {
            printf(2, "Bad thread count\n");
            exit();
        }

        long tcount = 0;
        // I'm trying to keep some presure off of malloc()
        victor_thread_t wthreads[MAX_THREADS];

        gen_data(lmatrix);
        gen_data(rmatrix);

        printf(1, "%s %d: %d\n", __FILE__, __LINE__, num_threads);
        omatrix->rows = lmatrix->rows;
        omatrix->cols = rmatrix->cols;
        omatrix->data = malloc(omatrix->rows * sizeof(MATRIX_TYPE *));

        //kdebug(2);
        if (lmatrix->cols == rmatrix->rows) {
            printf(1, "%s %d: %d\n", __FILE__, __LINE__, num_threads);

            for (tcount = 0; tcount < num_threads; tcount++) {
                victor_thread_create(&(wthreads[tcount]), cal_rowThreadStep
                               , (void *) tcount);
                printf(1, "  created thread %d %d\n"
                       , victor_thread_vid(wthreads[tcount]), tcount);
                if (victor_thread_vid(wthreads[tcount]) < 1) {
                    exit();
                }
            }
            cps();
            for (tcount = 0; tcount < num_threads; tcount++) {
                victor_thread_join(wthreads[tcount]);
                printf(1, "  joined thread %d %d\n"
                       , victor_thread_vid(wthreads[tcount]), tcount);
            }
        }
        else {
            printf(2, "*** the left matrix must have the "
                    "same number of colums as the right matrix has rows ***\n");
        }

        // its the output that takes most of the time
        output_matrix(omatrix, "op.txt");
        if (isVerbose) {
            output_matrix(lmatrix, "left.txt");
            output_matrix(rmatrix, "right.txt");
        }

        free_matrix(lmatrix);
        free_matrix(rmatrix);
        free_matrix(omatrix);
    }
    printf(1, "All tests passed\n");
#endif // VTHREADS

    exit();
}

#ifdef VTHREADS
void 
cal_rowThreadStep(void *arg)
{
    long tid = (long) arg;
    int row = tid;

    //printf(1, "thread: %s %d: %d\n", __FILE__, __LINE__, tid);

    // We are using static work allocation for this simple matrix
    // multiplication example.
    for (row = tid; row < omatrix->rows; row += num_threads) {
        calc_row(lmatrix, rmatrix, omatrix, row);
    }

    victor_thread_exit(7);
}

void
gen_data(matrix_t *matrix)
{
    matrix->rows = matrix->cols = MAX_MATRIX_SIZE;
    {
        uint row = 0;
        uint col = 0;
        int neg = 0;
        
        // instead of reading input from a file, which is really messy in xv6
        // we will just generate the input values

        matrix->data = malloc(matrix->rows * sizeof(MATRIX_TYPE *));
        for (row = 0; row < matrix->rows; row++) {
            matrix->data[row] = malloc(matrix->cols * sizeof(MATRIX_TYPE));
            for (col = 0; col < matrix->cols; col++) {
                // I want to have a mix of positive and negative values.
                // if the generated value is even, leave the number positive
                // the the number is odd, negate value
                neg = (myrand() % 2) == 0 ? 1 : -1;
                matrix->data[row][col] = (myrand() % 100) * neg;
            }
        }
    }
}

void
free_matrix(matrix_t *matrix)
{
    uint row = 0;

    if (matrix != NULL) {
        for (row = 0; row < matrix->rows; row++) {
            free(matrix->data[row]);
        }
        free(matrix->data);
        free(matrix);
    }
}

void
calc_row(matrix_t *lmat, matrix_t *rmat, matrix_t *omat, uint calc_row)
{
    uint lcol = 0;
    uint rcol = 0;
    MATRIX_TYPE sum = 0;

    omat->data[calc_row] = malloc(omat->cols * sizeof(MATRIX_TYPE));

    for (rcol = 0; rcol < rmat->cols; rcol++) {
        sum = 0;
        for (lcol = 0; lcol < lmat->cols; lcol++) {
            sum += lmat->data[calc_row][lcol] * rmat->data[lcol][rcol];
        }
        omat->data[calc_row][rcol] = sum;
    }
}

void
output_matrix(matrix_t *matrix, char *fname)
{
    int file = 1;
    uint row = 0;
    uint col = 0;

    if (NULL != matrix) {
        // always goes into the same file name
        file = open(fname, O_CREATE | O_RDWR);

        printf(file,  "%d %d\n", matrix->rows, matrix->cols);
        for (row = 0; row < matrix->rows; row++) {
            for (col = 0; col < matrix->cols; col++) {
                printf(file, "%d ", matrix->data[row][col]);
            }
            printf(file, "\n");
        }
        printf(file, "\n");

        close(file);
    }
}

#endif // VTHREADS
