#pragma once
//define을 여러줄하기 위해서 \ 을 써야함

//=====================================================
//                Crash
//컴파일러한테 nullptr이 아니라고 가정하라 요청함
//뒤에 값을 넣어 일부러 crash 냄
//=====================================================

#define CRASH(cause)                         \
{                                            \
  uint32* crash = nullptr;                   \
  __analysis_assume(crash != nullptr;);      \
  *crash = 0xDEADBEEF;                       \
}
//===================================================
//             조건부 Crash 
//==================================================
#define ASSERT_CRASH(expr)                       \
{                                                \
	if{!(expr))                                  \
    {                                            \
		CRASH("ASSERT_CRASH");                   \
       __analysis_assume(expr);                  \
	}                                            \
}