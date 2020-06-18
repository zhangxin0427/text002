void ADC_WatchdogConfig(void)
{
  ADC_AnalogWatchdogSingleChannelConfig(ADC1,ADC_Channel_0);
  ADC_AnalogWatchdogThresholdsConfig(ADC1,1500,0xFFF);
  ADC_AnalogWatchdogCmd(ADC1,ADC_AnalogWatchdog_SingleRegEnable);
}


void NVIC_Config(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_Init(&NVIC_InitStructure);
}


void ADC_IRQHandler(void)
{
  ADC_ITConfig(ADC1,ADC_IT_AWD,DISABLE);
  if(SET == ADC_GetFlagStatus(ADC1,ADC_FLAG_AWD))
  {
    ADC_ClearFlag(ADC1,ADC_FLAG_AWD);
    ADC_ClearITPendingBit(ADC1,ADC_IT_AWD);
      printf("ADC AWD is happened.\r\n");
   }
   ADC_ITConfig(ADC1,ADC_IT_AWD,ENABLE);
}
