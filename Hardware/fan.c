#include "fan.h"
#include "delay.h"

// ��ʼ���������ţ�PA11��
void FAN_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // ����GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // ����PA11Ϊ���������������ȸߵ�ƽ������
    GPIO_InitStructure.GPIO_Pin = FAN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FAN_GPIO, &GPIO_InitStructure);

    // Ĭ�Ϲرշ���
    FAN_Off();
}

// �������ȣ�PA11����ߵ�ƽ��
void FAN_On(void) {
    GPIO_SetBits(FAN_GPIO, FAN_PIN);
}

// �رշ��ȣ�PA11����͵�ƽ��
void FAN_Off(void) {
    GPIO_ResetBits(FAN_GPIO, FAN_PIN);
}

// ���ݱ�־λ���Ʒ���
void FAN_Control(uint8_t flag) {
    if (flag) {
        FAN_On();   // ����
    } else {
        FAN_Off();  // �ر�
    }
}

