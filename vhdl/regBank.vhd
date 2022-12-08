entity regBank is
    Port ( clk   : in  STD_LOGIC;
    Addr    : in  signed (4 downto 0);
    WriteAddr : in signed(4 downto 0);
    Write : in  STD_LOGIC;
    Din   : in  signed (31 downto 0);
    Dout  : out signed (31 downto 0)
    );
end regBank;

architecture behavior of regBank is
    type bankArr is array(0 to 31) of signed(Wortbreite-1 downto 0);
    signal bank : bankArr;   
    begin
      process begin
        wait until rising_edge(CLK);
        if (Write='1') then
          bank(to_integer(unsigned(WriteAddr))) <= Din;
        end if;
        Dout <= bank(to_integer(unsigned(Addr)));
      end process;
    end behavior;
