#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int x = is_mmio(addr);
  if(x==-1)
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  else
    return mmio_read(addr,len,x);
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int x = is_mmio(addr);
  if(x==-1)
    memcpy(guest_to_host(addr), &data, len);
  else
    mmio_write(addr,len,data,x);
}

// uint32_t vaddr_read(vaddr_t addr, int len) {
//   return paddr_read(addr, len);
// }

// void vaddr_write(vaddr_t addr, int len, uint32_t data) {
//   paddr_write(addr, len, data);
// }



// reference: nexus-am/am/arch/x86-nemu/include/x86.h
// +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |      Index     |                     |
// +----------------+----------------+---------------------+
//  \--- PDX(va) --/ \--- PTX(va) --/\------ OFF(va) ------/
#define PDX(va)     (((uint32_t)(va) >> 22) & 0x3ff)
#define PTX(va)     (((uint32_t)(va) >> 12) & 0x3ff)
#define OFF(va)     ((uint32_t)(va) & 0xfff)
#define PTE_ADDR(pte)   ((uint32_t)(pte) & ~0xfff)
paddr_t page_translate(vaddr_t addr, bool w1r0) {
    PDE pde, *pgdir;
    PTE pte, *pgtab;
    if (cpu.cr0.protect_enable && cpu.cr0.paging) {
	    pgdir = (PDE *)(PTE_ADDR(cpu.cr3.val)); 
	    pde.val = paddr_read((paddr_t)&pgdir[PDX(addr)], 4);
	    assert(pde.present);
	    pde.accessed = 1;

	    pgtab = (PTE *)(PTE_ADDR(pde.val));  
	    pte.val = paddr_read((paddr_t)&pgtab[PTX(addr)], 4);
	    assert(pte.present);
	    pte.accessed = 1;
	    pte.dirty = w1r0 ? 1 : pte.dirty; 

	    return PTE_ADDR(pte.val) | OFF(addr); 
	}

    return addr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  //data cross the page boundary
  if ((((addr) + (len) - 1) & ~PAGE_MASK) != ((addr) & ~PAGE_MASK)) {
    uint32_t data = 0;
    for(int i=0;i<len;i++){
      paddr_t paddr = page_translate(addr + i, false);
      data += (paddr_read(paddr, 1))<<8*i;
    }
    return data;
  } else {
    paddr_t paddr = page_translate(addr, false);
    return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  //data cross the page boundary
  if ((((addr) + (len) - 1) & ~PAGE_MASK) != ((addr) & ~PAGE_MASK)) {
    for(int i=0;i<len;i++){
      paddr_t paddr = page_translate(addr + i,true);
      paddr_write(paddr,1,data>>8*i);
    }
  } else {
    paddr_t paddr = page_translate(addr, true);
    paddr_write(paddr, len, data);
  }
}
