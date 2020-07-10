// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;
    
    pte_t page_table_entry = uvpt[PGNUM(addr)];
    envid_t env_id = sys_getenvid();


	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).
	// LAB 4: Your code here.

    if ((err & FEC_WR) == 0 || (page_table_entry & PTE_COW) == 0) {
        panic("pgfault: not a write or not to a COW page\n");
    }

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
    
    if ((r = sys_page_alloc(env_id, PFTEMP, PTE_W | PTE_U | PTE_P)) != 0) {
        panic("pgfault: page alloc fail! %e", r);
    }
    memcpy(PFTEMP, ROUNDDOWN(addr, PGSIZE), PGSIZE);
    if ((r = sys_page_map(env_id, PFTEMP, env_id, ROUNDDOWN(addr, PGSIZE), PTE_W | PTE_U | PTE_P)) != 0) {
        panic("pgfault: page map fail! %e", r);
    }
    if ((r = sys_page_unmap(env_id, PFTEMP)) != 0) {
        panic("pgfault: unmap fail! %e", r);
    }

	//panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
    
    envid_t parent_env_id = sys_getenvid();
    void *va = (void *)(pn * PGSIZE);

    if ((uvpt[pn] & PTE_W) == PTE_W || (uvpt[pn] & PTE_COW) == PTE_COW) {
        if ((r = sys_page_map(parent_env_id, va, envid, va, PTE_COW | PTE_U | PTE_P)) != 0) {
            panic("duppage: first mapping failed %e", r);
        }
        if ((r = sys_page_map(parent_env_id, va, parent_env_id, va, PTE_COW | PTE_U | PTE_P)) != 0) {
            panic("duppage: second mapping failed %e", r);
        }
    } else {
        if ((r = sys_page_map(parent_env_id, va, envid, va, PTE_U | PTE_P)) != 0) {
            panic("duppage: %e", r);
        }
    }

    return 0;
//	panic("duppage not implemented");
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//

/*
envid_t
fork(void)
{    
    set_pgfault_handler(pgfault);
    
    envid_t child_env_id = sys_exofork();    // child envid
    extern volatile pde_t uvpd[];
    extern volatile pte_t uvpt[];
    if (child_env_id < 0)
        return child_env_id;
    if (child_env_id == 0) {
        child_env_id = sys_getenvid();
        thisenv = &envs[ENVX(child_env_id)];
        return 0;
    }
    
    uint32_t addr;
    for (addr = 0; addr < USTACKTOP; addr += PGSIZE) {
        if ((uvpd[PDX(addr)] & PTE_P) == PTE_P && (uvpt[PGNUM(addr)] & PTE_P) == PTE_P) {
            duppage(child_env_id, PGNUM(addr));
        }
    }

    extern void _pgfault_upcall(void);
    
    int res;
    if ((res = sys_page_alloc(child_env_id, (void *)(UXSTACKTOP-PGSIZE), (PTE_U|PTE_W))) < 0)
        return res;

    sys_env_set_pgfault_upcall(child_env_id, _pgfault_upcall);
    
    sys_env_set_status(child_env_id, ENV_RUNNABLE);
    
    return child_env_id;
    
	//panic("fork not implemented");
}*/

envid_t
fork(void)
{
    
    // LAB 4: Your code here.
    envid_t env_id;
    uint32_t addr;
    int r;

    set_pgfault_handler(pgfault);
    env_id = sys_exofork();
    
    if (env_id < 0) {
        panic("fork: sys_exofork problem! %e", env_id);
    }
    if (env_id == 0) {
        thisenv = &envs[ENVX(sys_getenvid())];
        return 0;
    }

    for (addr = 0; addr < USTACKTOP; addr += PGSIZE) {
        if ((uvpd[PDX(addr)] & PTE_P) == PTE_P && (uvpt[PGNUM(addr)] & PTE_P) == PTE_P) {
            duppage(env_id, PGNUM(addr));
        }
    }

    // allocate new page for child's user exception stack
    void _pgfault_upcall();

    if ((r = sys_page_alloc(env_id, (void *)(UXSTACKTOP - PGSIZE), PTE_W | PTE_U | PTE_P)) != 0) {
        panic("fork: sys_page_alloc problem! %e", r);
    }
    if ((r = sys_env_set_pgfault_upcall(env_id, _pgfault_upcall)) != 0) {
        panic("fork: sys_env_set_pgfault_upcall problem! %e", r);
    }

    // mark the child as runnable
    if ((r = sys_env_set_status(env_id, ENV_RUNNABLE)) != 0)
        panic("fork: sys_env_set_status problem! %e", r);

    return env_id;
}



// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
