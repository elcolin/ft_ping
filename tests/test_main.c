#include "unity.h"
#include "../ft_ping.h"

void setUp()
{

}

void tearDown()
{

}

void test_initSocketFd(void)
{
    int sockfd = initSocketFd();
    TEST_ASSERT_NOT_EQUAL(-1, sockfd);
    close(sockfd);
}
void test_computeChecksum(void)
{
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    uint16_t checksum = computeChecksum(data, sizeof(data));
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, checksum);
}
void test_defineICMPHeader(void)
{
    struct icmp icmpHeader;
    defineICMPHeader(&icmpHeader);
    TEST_ASSERT_EQUAL(ICMP_ECHO, icmpHeader.icmp_type);
    TEST_ASSERT_EQUAL(ICMP_CODE, icmpHeader.icmp_code);
    TEST_ASSERT_EQUAL(getpid() & 0xFFFF, icmpHeader.icmp_id);
    TEST_ASSERT_EQUAL(ICMP_SEQUENCE_NUMBER, icmpHeader.icmp_seq);
    TEST_ASSERT_EQUAL_UINT16(0, icmpHeader.icmp_cksum);
}
void test_triggerError(void)
{
    int condition = 1;
    char *msg = "Test error message";
    triggerError(condition, msg);
    TEST_ASSERT_EQUAL(1, condition);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_initSocketFd);
    RUN_TEST(test_computeChecksum);
    RUN_TEST(test_defineICMPHeader);
    RUN_TEST(test_triggerError);
    return UNITY_END();
}