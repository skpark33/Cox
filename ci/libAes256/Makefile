include $(MAKEROOT)/stdmk
include $(MAKEROOT)/stdmk.cop

MODULE_DIR= caf/ci/libAes256

LIB_NAME = aes256

HDRS = \
	ciPadding_Type.h \
	ciByte_Block.h \
	ciS_Box.h \
	ciAES256_Base.h \
	ciAES256_PRNG.h \
	ciAES256.h \
	ciElCryptoAes256.h \
					
SRCS = \
	ciS_Box.h \
	ciAES256_Base.C \
	ciAES256_PRNG.C \
	ciAES256.C \
	ciElCryptoAes256.C \

OBJS = $(SRCS:.C=.o)

USER_INCLUDES 	+=
USER_LIBDIR		+=
USER_ENTRIES	+=

include $(MAKEROOT)/stdmk.lib
