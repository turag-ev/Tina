#include <tina++/debug.h>
#include <tina++/quad_encoder.h>

namespace TURAG {

void QuadEncoder::init()
{
    osalSysLock();

    if (config_.timer == STM32_TIM2) {
#ifdef STM32_TIM2_IS_USED
        turag_warning("QuadEncoder::init(): Timer 2 is already used!");
#endif
        rccEnableTIM2(FALSE);
        rccResetTIM2();
    } else if (config_.timer == STM32_TIM3) {
#ifdef STM32_TIM3_IS_USED
        turag_warning("QuadEncoder::init(): Timer 3 is already used!");
#endif
        rccEnableTIM3(FALSE);
        rccResetTIM3();
    } else if (config_.timer == STM32_TIM4) {
#ifdef STM32_TIM4_IS_USED
        turag_warning("QuadEncoder::init(): Timer 4 is already used!");
#endif
        rccEnableTIM4(FALSE);
        rccResetTIM4();
    } else if (config_.timer == STM32_TIM5) {
#ifdef STM32_TIM5_IS_USED
        turag_warning("QuadEncoder::init(): Timer 5 is already used!");
#endif
        rccEnableTIM5(FALSE);
        rccResetTIM5();
    } else {
        osalSysUnlock();
        turag_error("QuadEncoder::init(): Selected Timer is not supported!");
        return;
    }

    config_.timer->CR1  = 0;
    config_.timer->CR2  = 0;
    config_.timer->PSC  = 0;
    config_.timer->DIER = 0;
    config_.timer->ARR  = 0xFFFF;

    config_.timer->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;

    config_.timer->SMCR = TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0;

    if (config_.invert == InvertDirection::INVERT)
        config_.timer->CCER = TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC2E;
    else
        config_.timer->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E;
  
    config_.timer->CR1 = TIM_CR1_CEN;

    osalSysUnlock();
}


void QuadEncoder::measure()
{
    SystemTime now = SystemTime::now();
    Increments new_count = getIncrements();
    if (last_timestamp_ == SystemTime()) {
        // It's the first time, we cannot get a difference
        increment_count_ = new_count;
        last_timestamp_ = now;
    } else {
        last_timediff_ = now - last_timestamp_;
        last_timestamp_ = now;
        increment_diff_ = Increments(new_count - increment_count_);

        // check increment plausibility, can't happen on LMC
        // (unless the code above is buggy :>),
        // was due to shitty encoder counter on Localisation Steckboard
        if(std::abs(increment_diff_) > 2000)
            turag_criticalf("INCREMENT DIFF SHIT: old: %" PRIi16 ", new: %" PRIi16 ", diff: %" PRId32,
                            increment_count_, new_count, increment_diff_);

        increment_count_ = new_count;
    }
}


QuadEncoder::Increments QuadEncoder::getIncrements() const
{
    osalSysLock();
    Increments inc = config_.timer->CNT;
    osalSysUnlock();
    return inc;
}

}  // namespace TURAG
