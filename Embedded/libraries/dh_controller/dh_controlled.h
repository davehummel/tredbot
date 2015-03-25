#ifndef DH_CONTROLLED_H__
#define DH_CONTROLLED_H__

class Controlled{

public:

	virtual void begin(void)=0;
	virtual void execute(uint32_t time,char[] command)=0;
	virtual char[] serialize(void)=0;
	virtual void startSchedule(char[] command)=0;
	virtual void endSchedule(char[] command)=0;
	
	char[] error;
	
private:

};

	

#endif