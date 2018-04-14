

#ifndef     __MY_TYPE_H
#define     __MY_TYPE_H

#define   _var_size        0x4
#define   _pass_size       0xb


#define   t_none           0x0
#define   t_other          0x1
#define   t_other1         0x2
#define   t_other2         0x3

#define   t_push           0x1
#define   t_pop            0x2
#define   t_push           0x1
#define   t_pop            0x2
#define   t_add            0x4
#define   t_shl            0x1c
#define   t_shr            0x1d
#define   t_nand           0xf6

#define   t_byte           0x0
#define   t_word           0x1
#define   t_dword          0x2

#define   t_unkown         0xff

#define   t_lval           0xff

#define   t_reg_none       0xff
#define   t_reg_1          0x8
#define   t_reg_2          0x9
#define   t_reg_3          0x1

#define   t_init_disp_imm  0x0

#define   t_prefix_type    0x5
#define   t_prefix_es      0x0
#define   t_prefix_ss      0x0
#define   t_prefix_ds      0x0
#define   t_prefix_fs      0x0
#define   t_prefix_gs      0x0

#define   t_mem_type       0x3
#define   t_mem_es         0x0
#define   t_mem_cs         0x1
#define   t_mem_ss         0x2
#define   t_mem_ds         0x3
#define   t_mem_fs         0x4
#define   t_mem_gs         0x5

#define   t_cr_type        0x6
#define   t_cr0            0x0
#define   t_cr1            0x1
#define   t_cr2            0x2
#define   t_cr3            0x3
#define   t_cr4            0x4
#define   t_cr5            0x5
#define   t_cr6            0x6
#define   t_cr7            0x7

#define   t_dr_type        0x7
#define   t_dr0            0x0
#define   t_dr1            0x1
#define   t_dr2            0x2
#define   t_dr3            0x3
#define   t_dr4            0x4
#define   t_dr5            0x5
#define   t_dr6            0x6
#define   t_dr7            0x7


#define   t_retf           0xF7
#define   t_retn           0xF8



#endif      __MY_TYPE_H