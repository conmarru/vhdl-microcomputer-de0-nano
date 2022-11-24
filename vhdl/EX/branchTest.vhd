library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity brTest is
end entity brTest;

architecture testbench of brTest is
    component BRANCH is
        port (
            condition, program_counter : in signed(31 downto 0);
            cond_func : in signed(2 downto 0);
            offset : in signed(18 downto 0);
            output_address : out signed(31 downto 0)
        );
    end component BRANCH;
    signal condition, program_counter : signed(31 downto 0);
    signal cond_func : signed(2 downto 0);
    signal offset : signed(18 downto 0);
    signal output_address : signed(31 downto 0);

begin
  dlI: BRANCH
  port map (condition, program_counter, cond_func, offset, output_address);

  stiP: process is
  begin
    condition <= (others => '0');
    program_counter <= (others => '0');
    cond_func <= "100";
    offset <= to_signed(110, 32);
    wait for 10 ms;
    condition <= (1 => '1', others => '0');
    program_counter <= (others => '0');
    cond_func <= "100";
    offset <= "0000000000000000011";
    wait for 10 ms;
    condition <= (31 => '1', others => '0');
    program_counter <= (others => '0');
    cond_func <= "010";
    offset <= "0000000000000001001";
    wait for 10 ms;
    wait;
  end process stiP;

end architecture testbench;