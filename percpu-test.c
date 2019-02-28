#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/percpu.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/vmalloc.h>

#define MILLION 1000000
#define BILLION 1000000000

#define NUM_TESTS 3

/*
 * Module Parameters
 */
static int m_iterations = 1;
module_param(m_iterations, int, 0644);
MODULE_PARM_DESC(m_iterations, "Number of allocations (in millions).");

static int test_num = 1;
MODULE_PARM_DESC(test_num, "Test to run.");

/*
 * Test Legend
 * test1 - sequential deallocation
 * test2 - worst case alternate deallocation
 * test3 - deallocate reverse order
 */

/*
 * percpu test 1 - sequential deallocation
 */
static void percpu_test1(void)
{
	int i;
	int **arr;
	struct timespec start, end;

	arr = vmalloc(m_iterations * MILLION * sizeof(int *));

	pr_info("<percpu_test> percpu test 1: start\n");

	getnstimeofday(&start);

	for (i = 0; i < m_iterations * MILLION; i++)
		arr[i] = __alloc_percpu(16, 4);

	for (i = 0; i < m_iterations * MILLION; i++)
	        free_percpu(arr[i]);

	getnstimeofday(&end);

	pr_info("<percpu_test> percpu test 1 took: %ld ms\n",
	        ((end.tv_sec * BILLION + end.tv_nsec) -
	         (start.tv_sec * BILLION + start.tv_nsec)) / MILLION);

	vfree(arr);
}

/*
 * percpu test 2 - deallocate every other element
 */
static void percpu_test2(void)
{
	int i;
	int **arr;
	struct timespec start, end;

	arr = vmalloc(m_iterations * MILLION * sizeof(int *));

	pr_info("<percpu_test> percpu test 2: start\n");

	getnstimeofday(&start);

	for (i = 0; i < m_iterations * MILLION; i++)
		arr[i] = __alloc_percpu(16, 4);

	for (i = 0; i < m_iterations * MILLION; i += 2)
	        free_percpu(arr[i]);

	for (i = 1; i < m_iterations * MILLION; i += 2)
	        free_percpu(arr[i]);

	getnstimeofday(&end);

	pr_info("<percpu_test> percpu test 2 took: %ld ms\n",
	        ((end.tv_sec * BILLION + end.tv_nsec) -
	         (start.tv_sec * BILLION + start.tv_nsec)) / MILLION);

	vfree(arr);
}

/*
 * percpu test 3 - deallocate reverse order
 */
static void percpu_test3(void)
{
	int i;
	int **arr;
	struct timespec start, end;

	arr = vmalloc(m_iterations * MILLION * sizeof(int *));

	pr_info("<percpu_test> percpu test 3: start\n");

	getnstimeofday(&start);

	for (i = 0; i < m_iterations * MILLION; i++)
		arr[i] = __alloc_percpu(16, 4);

	for (i = m_iterations * MILLION - 1; i >= 0; i--)
	        free_percpu(arr[i]);

	getnstimeofday(&end);

	pr_info("<percpu_test> percpu test 3 took: %ld ms\n",
	        ((end.tv_sec * BILLION + end.tv_nsec) -
	         (start.tv_sec * BILLION + start.tv_nsec)) / MILLION);

	vfree(arr);
}

/*
 * kmalloc test 1 - sequential deallocation
 */
static void kmalloc_test1(void)
{
	int i;
	int **arr;
	struct timespec start, end;

	arr = vmalloc(m_iterations * MILLION * sizeof(int *));

	pr_info("<percpu_test> kmalloc test 1: start\n");

	getnstimeofday(&start);

	for (i = 0; i < m_iterations * MILLION; i++)
	        arr[i] = kmalloc(sizeof(int), GFP_KERNEL);

	for (i = 0; i < m_iterations * MILLION; i++)
	        kfree(arr[i]);

	getnstimeofday(&end);

	pr_info("<percpu_test> kmalloc test 1 took: %ld ms\n",
	        ((end.tv_sec * BILLION + end.tv_nsec) -
	         (start.tv_sec * BILLION + start.tv_nsec)) / MILLION);

	vfree(arr);
}

/*
 * kmalloc test 2 - deallocate every other element
 */
static void kmalloc_test2(void)
{
	int i;
	int **arr;
	struct timespec start, end;

	arr = vmalloc(m_iterations * MILLION * sizeof(int *));

	pr_info("<percpu_test> kmalloc test 2: start\n");

	getnstimeofday(&start);

	for (i = 0; i < m_iterations * MILLION; i++)
	        arr[i] = kmalloc(sizeof(int), GFP_KERNEL);

	for (i = 0; i < m_iterations * MILLION; i += 2)
	        kfree(arr[i]);

	for (i = 1; i < m_iterations * MILLION; i += 2)
	        kfree(arr[i]);

	getnstimeofday(&end);

	pr_info("<percpu_test> kmalloc test 2 took: %ld ms\n",
	        ((end.tv_sec * BILLION + end.tv_nsec) -
	         (start.tv_sec * BILLION + start.tv_nsec)) / MILLION);

	vfree(arr);
}

/*
 * kmalloc test 3 - deallocate reverse order
 */
static void kmalloc_test3(void)
{
	int i;
	int **arr;
	struct timespec start, end;

	arr = vmalloc(m_iterations * MILLION * sizeof(int *));

	pr_info("<percpu_test> kmalloc test 3: start\n");

	getnstimeofday(&start);

	for (i = 0; i < m_iterations * MILLION; i++)
	        arr[i] = kmalloc(sizeof(int), GFP_KERNEL);

	for (i = m_iterations * MILLION - 1; i >= 0; i--)
	        kfree(arr[i]);

	getnstimeofday(&end);

	pr_info("<percpu_test> kmalloc test 3 took: %ld ms\n",
	        ((end.tv_sec * BILLION + end.tv_nsec) -
	         (start.tv_sec * BILLION + start.tv_nsec)) / MILLION);

	vfree(arr);
}

/* Function pointer arrays to hold tests. */
static void (*percpu_tests[NUM_TESTS + 1]) (void) = {
	NULL,
	percpu_test1,
	percpu_test2,
	percpu_test3
};
static void (*kmalloc_tests[NUM_TESTS + 1]) (void) = {
	NULL,
	kmalloc_test1,
	kmalloc_test2,
	kmalloc_test3
};

/*
 * test driver
 *
 * This function is responsible for running the corresponding test cases
 * or all tests if 0 is passed.
 */
static void run_test(void)
{
	int i;

	pr_info("<percpu_test> m_iterations: %d\n", m_iterations);
	pr_info("<percpu_test> test_num: %d\n", test_num);

	if (test_num > 0 && test_num <= NUM_TESTS) {
	        (*percpu_tests[test_num])();
	        (*kmalloc_tests[test_num])();

	} else if (test_num == 0) {
	        for (i = 1; i <= NUM_TESTS; i++) {
	        	(*percpu_tests[i])();
	        	(*kmalloc_tests[i])();
	        }

	} else {
	        pr_info("<percpu_test> invalid test!\n");
	}
}

/*
 * kernel module callback
 */
int set_test_num(const char *val, const struct kernel_param *kp)
{
	int *pvalue = kp->arg;
	int res = param_set_ushort(val, kp);

	if (!res) {
	      	pr_info("<percpu_test> running test: %d\n", *pvalue);
	      	run_test();
	}
	return res;
}

const struct kernel_param_ops test_num_op_struct = {
	.set = &set_test_num,
	.get = &param_get_int,
};

module_param_cb(test_num,
		&test_num_op_struct,
		&test_num,
		0644);

/*
 * percpu_test module init
 */
static int __init percpu_test_init(void)
{
	pr_info("<percpu_test> init percpu test module\n");

	return 0;
}

/*
 * percpu_test module cleanup
 */
static void __exit percpu_test_exit(void)
{
	pr_info("<percpu_test> removed percpu test module\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dennis Zhou <dennis@kernel.org>");


module_init(percpu_test_init);
module_exit(percpu_test_exit);
