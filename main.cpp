#include <cstdio>
#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <memory>

int ChildDo(std::shared_ptr<int> ptr)
{
    printf("i am a  child, pid: %d, testPtr:%p\n", getpid(), ptr.get());
    sleep(10);
    return 0;
}

int ParentDo(const int childPid, std::shared_ptr<int> ptr)
{
    printf("i am a parent, pid: %d, testPtr:%p\n", getpid(), ptr.get());

    int childStatus = 0;
    // wait(&childStatus) は waitpid(-1, &childStatus, 0) と等価
    const auto waitRet = wait(&childStatus);
    if (waitRet == -1)
    {
        perror("wait error\n");
        return 1;
    }

    if (waitRet != childPid)
    {
        const std::string err = "子プロセスの id " + std::to_string(childPid) + ", " + std::to_string(waitRet) + " が一致しません\n";
        perror(err.c_str());
        return 1;
    }

    if (WIFEXITED(childStatus))
    {
        printf("子プロセスが正常終了しました %d\n", WEXITSTATUS(childStatus));
    }
    if (WIFSIGNALED(childStatus))
    {
        printf("子プロセスがシグナルにより終了しました %d\n", WTERMSIG(childStatus));
    }

    return 0;
}

int main()
{
    printf("プロセス開始\n");
    auto pTest = std::make_shared<int>(100);

    // fork() は呼び出し元プロセスを複製して新しいプロセスを生成する
    // child プロセスは、基本的に parent プロセスの複製 -> 変数の値やポインタのアドレスも同じ
    // 複製できない部分もあるので詳細は `man fork` で確認すること
    const auto pid = fork();
    if (pid == -1)
    {
        perror("fork error\n");
        return 1;
    }

    // ここから child プロセスの動作が開始する
    if (pid == 0)
    {
        return ChildDo(pTest);
    }

    return ParentDo(pid, pTest);
}