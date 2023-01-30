library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use STD.TEXTIO.all;

-- yoinked and adapted from lothar-miller.de
entity RAM is
    Port ( clk   : in  STD_LOGIC;
           reset : in std_logic;
           Addr     : in  signed (31 downto 0);
           write_in : in  STD_LOGIC;
           Din   : in  signed (31 downto 0);
           Dout  : out signed (31 downto 0)
         );
end RAM;

architecture BlockRAM of RAM is
    type speicher is array(0 to 16368) of signed(31 downto 0);
    signal memory : speicher;   

    file data_in : TEXT;


    begin
      behav : process (clk, Addr, reset) is 
          variable cur_read : line;
          variable int : integer;
          variable x : integer := 0;
          variable good : boolean;
      begin
        if reset = '1' then
          file_open(data_in, "instructions.bin", read_mode);
          while not endfile (data_in) loop
            readline(data_in, cur_read);
            read(cur_read, int);
            report integer'image(int);
            memory(x) <= to_signed(int, 32);
            x := x + 1;
          end loop;
          file_close(data_in);
        end if;
        report to_string(Addr);
        if rising_edge(clk) then
          if (write_in = '1') then
          report "Trying write";
          if to_integer(Addr) < 16368 then
            memory(to_integer(Addr)) <= Din;
          else
            report "Out of bounds memory read";
          end if;
          end if;
        end if;
        if to_integer(Addr) < 16368 and to_integer(Addr) >= 0 then
          Dout <= memory(to_integer(Addr));
        else
          report "Out of bounds memory read";
        end if;
      end process;
    end BlockRAM;