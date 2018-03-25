#include "MainApp.h"
MainAppInstanceData_t MainAPP;
MainAppInstanceData_t *pMainAPP;

bool_t  AppModuleRegisterInit(void )
{
    bool_t      success = false;
 // tasksEvents = (uint16_t *)malloc( sizeof( uint16_t ) * 10);
//  osal_memset( tasksEvents, 0, (sizeof( uint16_t ) * 10));
    pMainAPP=&MainAPP;

utilVariableInit();  //MESSAGE SEND INT

#if INC_UART
    Init_UerUART_module(mqUART);
#endif 
    UART1_SendData2PC("System Initializing......",strlen("System Initializing......"));

#if INC_LCD
    Init_Lcd_module(mqLCD);
#endif 

#if INC_NVRAM
    Init_NVRAM_module(mqNVRAM);
#endif 

#if INC_KEY
     Init_Key_module(mqKEY);
#endif 

#if INC_LED
    Init_Led_module(mqLED);
#endif 

#if INC_IR
    Init_IR_module(mqIR);
#endif 

#if INC_CPLD
    Init_cpld_module(mqCPLD);
#endif 

#if INC_INPUT_VIDEO
    Init_InputVideo_module(mqINPUT);
#endif 

#if INC_SWITCH_VIDEO
    Init_SwitchVideo_module(mqSWITCH);
#endif 

#if INC_OUTPUT_VIDEO
    //Init_OutPutVideo_module(mqOUTPUT);
#endif 
    return success;
}

//------------------------------------------------------------------------------
// Function:    AppCallbackRegisterParser
// Description: Register a message handler with the specified function instance
// Parameters:  Callback function pointer
// Returns:     true if successful
//------------------------------------------------------------------------------

bool_t AppCallbackRegisterParser (eMESSAGE_QUEUES  DriverMsgId ,SendMsgHandler pCallBack,   bool_t callAlways)
{
    bool_t  success = false;
 //   int_t   callBackIndex;
    ASSERT(DriverMsgId<MAX_MAINAPP_PARSERS);
	
//    for ( callBackIndex = 0; callBackIndex < MAX_MAINAPP_PARSERS; callBackIndex++ )
  //  {
        if ( pMainAPP->messageHandlers[ DriverMsgId ].pMsgHandler == 0)
        {
           
            pMainAPP->messageHandlers[ DriverMsgId ].pMsgHandler  = pCallBack;//pCallBack;  // SiiCecMsgHandlers_t
            pMainAPP->messageHandlers[ DriverMsgId ].callAlways   = callAlways;
	     pMainAPP->messageHandlers[ DriverMsgId ].DriverMsgId= DriverMsgId;
            success = true;
           // break;
        }
	// else
	//  DEBUG_PRINT("AppCallbackRegisterParser  already  Register !\r\n");
	 
    //}

    return( success );
}

//------------------------------------------------------------------------------
// Function:    AppCallbackRegisterTask
// Description: Register a  task with the current  instance
// Parameters:  Callback function pointer
// Returns:     Index of registered task, or -1 if no room
//------------------------------------------------------------------------------

int_t AppCallbackRegisterTask ( eMESSAGE_QUEUES  DriverMsgId,bool_t (*pCallBack)(), bool_t callAlways )
{
    int_t callBackIndex;

  //  for ( callBackIndex = 0; callBackIndex < MAX_MAINAPP_TASKS; callBackIndex++ )
  //  {
        if ( pMainAPP->TasksHandlers[ DriverMsgId ].pTasks== 0)  //before not regist
        {
            pMainAPP->TasksHandlers[ DriverMsgId ].pTasks = pCallBack;
	     pMainAPP->TasksHandlers[ DriverMsgId ].callAlways   = callAlways;
	     pMainAPP->TasksHandlers[ DriverMsgId ].DriverMsgId = DriverMsgId;
        //    break;
        }
  //  }

    return( ( callBackIndex == MAX_MAINAPP_TASKS) ? -1 : callBackIndex) ;
}

#if 0
BOOL utilExecMsgPresent(eMESSAGE_QUEUES eQueue)
{
    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);

    return !m_asQInfo[eQueue].cQueueEmpty;
}
#endif
//------------------------------------------------------------------------------
// Function:    SiiCecHandler
// Description: Polls the send/receive state of the CPI hardware and runs the
//              current task, if any.
//
//              A task is used to handle cases where we must send and receive
//              a specific set of CEC commands.
//------------------------------------------------------------------------------

void AppTaskMsgLoop (  void  )
{
    bool_t          processedMsg;
    int_t           callbackIndex, frameCount;
    SendComandState_t  Status;
    SendData_t    Frame;
   // do
    {
    #if 0
		/* ---------------------------- END CUSTOM AREA --------------------------- */

		// check if user input control has a new keypress
		if (utilExecMsgPresent(mqUSER_INPUT))
		{
		utilExecMsgRead(mqUSER_INPUT,
		(BYTE *)&sKeyEvent);
		//DEBUG_PRINT(MSG_ERR, "***USER_INPUT = %d\r\n",  sKeyEvent.eKey);
		/* Add code here to perform a post read operation on the user input queue */
		utilExecMsgClear(mqUSER_INPUT);
		/* ---------------------------- END CUSTOM AREA --------------------------- */
		}
		else
	          break;
   #endif
                //loop all tasks if task  resgit
               for ( callbackIndex = 0; callbackIndex < MAX_MAINAPP_TASKS; callbackIndex++ )
                {
                    if ( pMainAPP->TasksHandlers[ callbackIndex].pTasks!= 0) //if task  resgit
                    {
                        // Some handlers must be called every time through the handler.
                         if (pMainAPP->TasksHandlers[ callbackIndex].callAlways )
                         {
                               pMainAPP->TasksHandlers[ callbackIndex].pTasks();  //execute task
                         }
                 
                    }
                }
   
	  //check message FIFO have  new  message

	  
                // Iterate through registered message handlers.
                for ( callbackIndex = 0; callbackIndex < MAX_MAINAPP_PARSERS; callbackIndex++ )
                {
                    if ( pMainAPP->messageHandlers[ callbackIndex].pMsgHandler != 0) //if task  resgit
                    {
                        // Some handlers must be called every time through the handler.
                        if ( pMainAPP->messageHandlers[ callbackIndex].callAlways )
                        {
                             (*pMainAPP->messageHandlers[ callbackIndex].pMsgHandler)( &Frame ); // process
                          //      break;
                          
                        } 
                    }
                }
          
   } 
        //while ( 0 );
}


void AppTasKeyLedMsgLoop (  void  )
{
    bool_t          processedMsg;
    int_t           callbackIndex, frameCount;
    SendComandState_t  Status;
    SendData_t    Frame;
   // do
    {
                //loop all tasks if task  resgit
               for ( callbackIndex = mqLED; callbackIndex < mqUART; callbackIndex++ )
                {
                    if ( pMainAPP->TasksHandlers[ callbackIndex].pTasks!= 0) //if task  resgit
                    {
                        // Some handlers must be called every time through the handler.
                         if (pMainAPP->TasksHandlers[ callbackIndex].callAlways )
                         {
                               pMainAPP->TasksHandlers[ callbackIndex].pTasks();  //execute task
                         }
                 
                    }
                }
   
	  //check message FIFO have  new  message

	  
                // Iterate through registered message handlers.
                for ( callbackIndex = mqLED; callbackIndex < mqUART; callbackIndex++ )
                {
                    if ( pMainAPP->messageHandlers[ callbackIndex].pMsgHandler != 0) //if task  resgit
                    {
                        // Some handlers must be called every time through the handler.
                        if ( pMainAPP->messageHandlers[ callbackIndex].callAlways )
                        {
                             (*pMainAPP->messageHandlers[ callbackIndex].pMsgHandler)( &Frame ); // process
                          //      break;
                          
                        } 
                    }
                }
          
   } 
        //while ( 0 );
}

//------------------------------------------------------------------------------
// Function:    AppTaskLoop
// Description: CEC control task.  Emulate a task with a blocking semaphore
//              waiting for a device interrupt or a timer-based poll request
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------
/*
void AppTaskMsgLoop (void )
{
        AppHandler();
}*/
#if 0
/*********************************************************************
 * @fn      osal_init_system
 *
 * @brief
 *
 *   This function initializes the "task" system by creating the
 *   tasks defined in the task table (OSAL_Tasks.h).
 *
 * @param   void
 *
 * @return  SUCCESS
 */
uint8 osal_init_system( void )
{
  // Initialize the Memory Allocation System
  osal_mem_init();

  // Initialize the message queue
  osal_qHead = NULL;

  // Initialize the timers
  osalTimerInit();

  // Initialize the Power Management System
  osal_pwrmgr_init();

  // Initialize the system tasks.
  osalInitTasks();

  // Setup efficient search for the first free block of heap.
  osal_mem_kick();

  return ( SUCCESS );
}
/*********************************************************************
 * @fn      osal_start_system
 *
 * @brief
 *
 *   This function is the main loop function of the task system (if
 *   ZBIT and UBIT are not defined). This Function doesn't return.
 *
 * @param   void
 *
 * @return  none
 */
void osal_start_system( void )
{
#if !defined ( ZBIT ) && !defined ( UBIT )
  for(;;)  // Forever Loop
#endif
  {
    osal_run_system();
  }
}

/*********************************************************************
 * @fn      osal_run_system
 *
 * @brief
 *
 *   This function will make one pass through the OSAL taskEvents table
 *   and call the task_event_processor() function for the first task that
 *   is found with at least one event pending. If there are no pending
 *   events (all tasks), this function puts the processor into Sleep.
 *
 * @param   void
 *
 * @return  none
 */
void osal_run_system( void )
{
  uint8 idx = 0;

  Hal_ProcessPoll(); //

  do {
    if (tasksEvents[idx])  // Task is highest priority that is ready.
    {
      break;
    }
  } while (++idx < tasksCnt);

  if (idx < tasksCnt)
  {
    uint16 events;
   // halIntState_t intState;

   // HAL_ENTER_CRITICAL_SECTION(intState);
    events = tasksEvents[idx];
    tasksEvents[idx] = 0;  // Clear the Events for this task.
   // HAL_EXIT_CRITICAL_SECTION(intState);

    activeTaskID = idx;
    events = (tasksArr[idx])( idx, events );
    activeTaskID = TASK_NO_TASK;

  //  HAL_ENTER_CRITICAL_SECTION(intState);
    tasksEvents[idx] |= events;  // Add back unprocessed events to the current task.
  //  HAL_EXIT_CRITICAL_SECTION(intState);
  }

}

/*********************************************************************
 * @fn      osal_int_disable
 *
 * @brief
 *
 *   This function is called to disable an interrupt. When a disabled
 *   interrupt occurs, the service routine associated with that
 *   interrupt is not called.
 *
 *   If INTS_ALL is the interrupt_id, interrupts (in general) are disabled.
 *   If a single interrupt is passed in, then just that interrupt is disabled.
 *
 * @param   uint8 interrupt_id - Interrupt number
 *
 * @return  SUCCESS or INVALID_INTERRUPT_ID
 */
uint8 osal_int_disable( uint8 interrupt_id )
{

  if ( interrupt_id == INTS_ALL )
  {
    HAL_DISABLE_INTERRUPTS();
    return ( SUCCESS );
  }
  else
  {
    return ( INVALID_INTERRUPT_ID );
  }
}
/*********************************************************************
 * @fn      osal_int_enable
 *
 * @brief
 *
 *   This function is called to enable an interrupt. Once enabled,
 *   occurrence of the interrupt causes the service routine associated
 *   with that interrupt to be called.
 *
 *   If INTS_ALL is the interrupt_id, interrupts (in general) are enabled.
 *   If a single interrupt is passed in, then interrupts still have
 *   to be enabled with another call to INTS_ALL.
 *
 * @param   uint8 interrupt_id - Interrupt number
 *
 * @return  SUCCESS or INVALID_INTERRUPT_ID
 */
uint8 osal_int_enable( uint8 interrupt_id )
{

  if ( interrupt_id == INTS_ALL )
  {
    HAL_ENABLE_INTERRUPTS();
    return ( SUCCESS );
  }
  else
  {
    return ( INVALID_INTERRUPT_ID );
  }
}

#endif
