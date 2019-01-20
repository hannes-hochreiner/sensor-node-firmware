#include "irq_handler.h"

static void_func dma_c1 = 0;
static void_func dma_c2_c3 = 0;
static void_func dma_c4_c5 = 0;

result_t add_handler(IRQn_Type num, void_func func) {
  switch (num) {
    case DMA1_Channel1_IRQn:
      if (dma_c1 != 0) {
        return RESULT_ERROR;
      }

      dma_c1 = func;

      return RESULT_OK;
    case DMA1_Channel2_3_IRQn:
      if (dma_c2_c3 != 0) {
        return RESULT_ERROR;
      }

      dma_c2_c3 = func;

      return RESULT_OK;
    case DMA1_Channel4_5_IRQn:
      if (dma_c4_c5 != 0) {
        return RESULT_ERROR;
      }

      dma_c4_c5 = func;

      return RESULT_OK;
    default:
      return RESULT_ERROR;
  }
}

result_t remove_handler(IRQn_Type num, void_func func) {
  switch (num) {
    case DMA1_Channel1_IRQn:
      if (dma_c1 != func) {
        return RESULT_ERROR;
      }

      dma_c1 = 0;

      return RESULT_OK;
    case DMA1_Channel2_3_IRQn:
      if (dma_c2_c3 != func) {
        return RESULT_ERROR;
      }

      dma_c2_c3 = 0;

      return RESULT_OK;
    case DMA1_Channel4_5_IRQn:
      if (dma_c4_c5 != func) {
        return RESULT_ERROR;
      }

      dma_c4_c5 = 0;

      return RESULT_OK;
    default:
      return RESULT_ERROR;
  }
}

void DMA1_Channel1_IRQHandler() {
  if (dma_c1 != 0) {
    (*dma_c1)();
  }
}

void DMA1_Channel2_3_IRQHandler() {
  if (dma_c2_c3 != 0) {
    (*dma_c2_c3)();
  }
}

void DMA1_Channel4_5_IRQHandler() {
  if (dma_c4_c5 != 0) {
    (*dma_c4_c5)();
  }
}
