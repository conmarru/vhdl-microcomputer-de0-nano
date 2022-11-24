unsigned int get_hash(char* str, unsigned int div)
{
	unsigned int i = 0; 
	unsigned int hash = 0; 

	while (str[i] != 0x00)
	{
		hash += str[i] + i;
		i++;
	}
	
	return hash % div;
}
