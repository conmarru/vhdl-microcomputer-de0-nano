ghdl -a --std=08 --workdir=work arch_constants.vhd
for file in alu.vhd branch.vhd pre_exec_reg.vhd pre_fetch_reg.vhd pre_mem_reg.vhd pre_wb_reg.vhd program_counter.vhd ram.vhd reg_bank.vhd cpu.vhd; do
  ghdl -a --std=08 --workdir=work $file
  ghdl -e --std=08 --workdir=work ${file%.*}
done
