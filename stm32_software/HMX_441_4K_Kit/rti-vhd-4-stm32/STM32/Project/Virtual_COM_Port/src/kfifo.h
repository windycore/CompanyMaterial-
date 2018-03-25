
//[kyq Begin: 20151123 14:55:04]

#define	KFIFO_BASE_SIZE					12
typedef struct kfifo
{
    //unsigned char *buffer;    /* the buffer holding the data */  move last
    
    unsigned int in;    /* data is added at offset (in % size) */   //  inָ��buffer�ж�ͷ
    unsigned int out;    /* data is extracted from off. (out % size) */     //outָ��buffer�еĶ�β
    unsigned int size;    /* the size of the allocated buffer */
    //spinlock_t *lock;    /* protects concurrent modifications */ //ֻ��һ������̺߳�һ�������̵߳ĳ���ʱ�������߳̿��Բ���������������Ҫ�κμ�����Ϊ
	//unsigned char *buffer;	  /* the buffer holding the data */  // Move to here
	unsigned char	buffer[1];

}kfifo_t;


#define kfifo_len(fifo)					((fifo)->in - (fifo)->out)
#define	kfifo_unused(fifo)				((fifo)->size �C (fifo)->in + (fifo)->out)
#define	kfifo_is_empty(fifo)			((fifo)->in == (fifo)->out)
//#define	kfifo_is_full(fifo)				(kfifo_len(fifo) > (fifo)->mask)
void kfifo_free(kfifo_t *fifo, unsigned char flag);
kfifo_t *kfifo_alloc(unsigned char* buffer,unsigned int size,unsigned char flag );   
unsigned int kfifo_put(struct kfifo *fifo,   
             unsigned char *buffer, unsigned int len);   
unsigned int kfifo_get(struct kfifo *fifo,   
						  unsigned char *buffer, unsigned int len);	 
unsigned int kfifo_out_peek(kfifo_t *fifo, void *buf, unsigned int len,unsigned int offset);

unsigned int kfifo_out_seek(kfifo_t *fifo, unsigned int len);

