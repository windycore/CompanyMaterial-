
#include "kfifo.h"
#define MIN(a,b)  (((a) > (b)) ? (b) : (a))
#define MAX(a,b)  (((a) > (b)) ? (a) : (b))

// refer :http://blog.csdn.net/summerhust/article/details/7268766 
//        http://www.cnblogs.com/Anker/p/3481373.html 
//        http://blog.chinaunix.net/uid-18770639-id-4203078.html 
void kfifo_free(kfifo_t *fifo, unsigned char flag)
{
	//printf("kfifo_free\r\n");
	
	if(flag)
	{
		free(fifo);
	}
} 
//fifo->in �C fifo->out��ʾ����������ʹ�õĿռ��С
//fifo->size - (fifo->in �C fifo->out)��ʾ����δʹ�õĿռ�
// 
//  flag = 1: use dyn memory
//       = 0: use static memory
// 
kfifo_t *kfifo_alloc(unsigned char* buffer,unsigned int size,unsigned char flag )   
{   
      
    kfifo_t *ret;
  
    /*  
     * round up to the next power of 2, since our 'let the indices  
     * wrap' tachnique works only in this case.  
     */   
    if (size & (size - 1))
    {
        //��ʾsize����2���� 
        PRINTF("Error FIFO SIZE");
        //�����Ļ� kfifo->in % kfifo->size ����ת��Ϊ kfifo->in & (kfifo->size �C 1) 
        return NULL;  
    }   
  
    
     if (flag)
    {
        ret = (kfifo_t*) malloc(sizeof(KFIFO_BASE_SIZE+size));
    }  
    else
    {
        ret = (kfifo_t*)buffer;
    }

    
  
    //ret->buffer = buffer;
    ret->size = size;
    ret->in = ret->out = 0;
    return ret;   
}    

// ���ڿ��г��ȵ����ݲ��´��, ���Ḳ��ԭ��������
//  ���� ���� �� ���ݳ���
unsigned int kfifo_put(struct kfifo *fifo,   
             unsigned char *buffer, unsigned int len)   
{   
    unsigned int l;   
  
    len = MIN(len, fifo->size - fifo->in + fifo->out);   
    if (len==0)
    {
       return 0;
    }
    /*  
     * Ensure that we sample the fifo->out index -before- we  
     * start putting bytes into the kfifo.  
     */
  
    //smp_mb();   
  
    /* first put the data starting from fifo->in to buffer end */ 
    //kfifo->size �C kfifo->in & (kfifo->size �C 1) ��ʾin�±굽bufferĩβ?���ж��ٿռ�  
    l = MIN(len, fifo->size - (fifo->in & (fifo->size - 1)));   
    memcpy((unsigned char *)(fifo->buffer + (fifo->in & (fifo->size - 1))), buffer, l);   
  
    /* then put the rest (if any) at the beginning of the buffer */   
    memcpy(fifo->buffer, buffer + l, len - l);   
  
    /*  
     * Ensure that we add the bytes to the kfifo -before-  
     * we update the fifo->in index.  
     */   
  
    //smp_wmb();   
  
    fifo->in += len;   
  
    return len;   
}  
// ���ֻ�ܿ��� BUffer �е����ݳ���
//  ���� ���� �� ���ݳ���  , fifo ���� ��С
unsigned int kfifo_get(struct kfifo *fifo,   
             unsigned char *buffer, unsigned int len)   
{   
    unsigned int l;   
    // ��Ҫ�������ݳ��Ⱥ� fifo ���е����ݳ��� �Ƚ� ȡС��
    len = MIN(len, fifo->in - fifo->out);   //fifo->in �C fifo->out��ʾ����������ʹ�õĿռ��С
    //kyq
    if (len == 0)
    {
        return 0;
    }
    /*  
     * Ensure that we sample the fifo->in index -before- we  
     * start removing bytes from the kfifo.  
     */
  
    //smp_rmb();   
   
    /* first get the data from fifo->out until the end of the buffer */ 
    l = MIN(len, fifo->size - (fifo->out & (fifo->size - 1)));   
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);   
  
    /* then get the rest (if any) from the beginning of the buffer */   
    memcpy(buffer + l, fifo->buffer, len - l);   
  
    /*  
     * Ensure that we remove the bytes from the kfifo -before-  
     * we update the fifo->out index.  
     */   
  
    //smp_mb();   
  
    fifo->out += len;   
  
    return len;   
}   
static void kfifo_copy_out(kfifo_t *fifo, unsigned char *dst,		unsigned int len, unsigned int off)
{
#if 1
   unsigned int size = fifo->size;//fifo->mask + 1;	
   //unsigned int esize = fifo->esize;	
   unsigned int l;	
   off &= (fifo->size-1);//fifo->mask;// it is;	off = off&(fifo->size-1)   	
   l = MIN(len, size - off);	
   memcpy(dst, fifo->buffer + off, l);	
   memcpy((unsigned char *)(dst + l), fifo->buffer, len - l);	/*	 * make sure that the data is copied before	 * incrementing the fifo->out index counter	 */
   //smp_wmb();
#else
unsigned int l;   
// ��Ҫ�������ݳ��Ⱥ� fifo ���е����ݳ��� �Ƚ� ȡС��
len = MIN(len, fifo->in - fifo->out);	//fifo->in �C fifo->out��ʾ����������ʹ�õĿռ��С
//kyq
if (len == 0)
{
	return 0;
}
/*	
 * Ensure that we sample the fifo->in index -before- we  
 * start removing bytes from the kfifo.  
 */

//smp_rmb();   

/* first get the data from fifo->out until the end of the buffer */ 
l = MIN(len, fifo->size - (fifo->out & (fifo->size - 1)));	 
memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);	

/* then get the rest (if any) from the beginning of the buffer */	
memcpy(buffer + l, fifo->buffer, len - l);	 

/*	
 * Ensure that we remove the bytes from the kfifo -before-	
 * we update the fifo->out index.  
 */   

//smp_mb();   


#endif
}


unsigned int kfifo_out_peek(kfifo_t *fifo, void *buf, unsigned int len,unsigned int offset)
{	
   unsigned int l;
   if(offset > (fifo->in - fifo->out))
   	return 0;
   l = fifo->in - (fifo->out+offset);
   if (len > l)
   len = l;
   kfifo_copy_out(fifo, buf, len, fifo->out+offset);
   return len;
}

unsigned int kfifo_out_seek(kfifo_t *fifo, unsigned int len)
{
	unsigned int l;

	l = fifo->in - fifo->out;
	if (len > l)
		len = l;
	fifo->out += len;
	return len;
}


