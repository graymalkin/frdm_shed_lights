//I2C i2c(p28, p27);
 
void init_dio();
void pwm_off(void const *arg);
 
void cmd_i2c_write(char* qry, char* data, char* resp);
void cmd_i2c_read(char* qry, char* data, char* resp);
void cmd_i2c_addr_read(char* qry, char* data, char* resp);
void cmd_pmd_setio(char* qry, char* data, char* resp);
void cmd_pmd_write(char* qry, char* data, char* resp);
void cmd_pmd_read(char* qry, char* data, char* resp);
void cmd_pwm_set(char* qry, char* data, char* resp);
void muri(char* uri, char* data, char* resp);