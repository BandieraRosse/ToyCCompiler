/* EXPECT_ERR: unterminated character constant */
int main() {
    char c = 'a;
    return 0;
}
