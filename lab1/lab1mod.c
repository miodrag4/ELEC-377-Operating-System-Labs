//Libraries

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/ktime.h>


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
#define HAVE_PROC_OPS
#endif

//Global Variables and lab1_show stucture declaration

static int lab1_show(struct seq_file *m, void *v) {
  struct task_struct * cur_task = current;

//State initialize statemens plus the pointer set statement

 int state = current -> state;

//PID AND PPID initialize statements plus the pointers set statements

  int PID = current -> pid; 
  int PPID = task_ppid_nr(current);

//UID AND GID initialize statements plus the pointers set statements

  int RealUID = current -> cred -> uid.val;
  int EffectiveUID = current -> cred -> euid.val;
  int SavedUID = current -> cred -> suid.val;
  int RealGID = current -> cred -> gid.val;
  int EffectiveGID = current -> cred -> egid.val;
  int SavedGID = current -> cred -> sgid.val;

//intalize statements for Mathematical Equation

  int hrs,mins,secs;
  s64 secondsup;

//Mathematical Equation for calculating the time the system has been up

  secondsup = ktime_divns(ktime_get_coarse_boottime(), NSEC_PER_SEC);
  
  hrs = secondsup / 3600;
  mins = (secondsup % 3600) / 60;
  secs = secondsup % 60;

//Print statements for the desired variables

  seq_printf(m,"System up(%lld): %d hrs, %d mins, %d seconds\n", secondsup, hrs, mins,secs);
  seq_printf(m,"Current Process PCB Information\n");
  seq_printf(m,"Name = %s\n",current -> comm);
  seq_printf(m,"PID = %d\n", PID);
  seq_printf(m,"PPID = %d\n", PPID);

//Conditional "if" statements to print the state
//The the T.A approved the use of the field instead long variable values defined in the file.

  if(state == TASK_RUNNING){
    seq_printf(m,"State = Running\n");
  }
  else if(state == TASK_INTERRUPTIBLE || TASK_UNINTERRUPTIBLE){
      seq_printf(m,"State = Waiting\n");
  }
  else{
    seq_printf(m,"State = Stopped\n");
  }

  seq_printf(m,"Real UID = %d\n", RealUID );
  seq_printf(m,"Effective UID = %d\n", EffectiveGID);
  seq_printf(m,"Saved UID = %d\n", SavedUID);
  seq_printf(m,"Real GID = %d\n", RealGID );
  seq_printf(m,"Effective UID = %d\n", EffectiveGID);
  seq_printf(m,"Saved GID = %d\n", SavedGID );
  return 0;
}

static int lab1_open(struct inode *inode, struct  file *file) {
  return single_open(file, lab1_show, NULL);
}

//Structure fields for the specific functions to be called

#ifdef HAVE_PROC_OPS
static const struct proc_ops lab1_fops = {
  .proc_open = lab1_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,
};
#else
static const struct file_operations lab1_fops = {
  .owner = THIS_MODULE,
  .open = lab1_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};
#endif

//Functions lab1_init and lab1_exit

static int __init lab1_init(void) {
  proc_create("lab1", 0, NULL, &lab1_fops);
  printk(KERN_INFO "lab1mod in\n");
  return 0;
}

static void __exit lab1_exit(void) {
  remove_proc_entry("lab1", NULL);
  printk(KERN_INFO "lab1mod out\n");
}

//Function Reference for the module

MODULE_LICENSE("GPL");
module_init(lab1_init);
module_exit(lab1_exit);