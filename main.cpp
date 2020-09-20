#include <cstdio>
#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <memory>

int ChildDo(std::shared_ptr<int> ptr)
{
    const auto pid = getpid();
    printf("i am a child, pid: %d, ptr:%p\n", pid, ptr.get());
    sleep(10);
    return 0;
}

int ParentDo(const int childPid, std::shared_ptr<int> ptr)
{
    const auto pid = getpid();
    printf("i am a parent, pid: %d, ptr:%p\n", pid, ptr.get());

    int status = 0;
    // wait(&status) は waitpid(-1, &status, 0) と等価
    const auto waitRet = wait(&status);
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

    if (WIFEXITED(status))
    {
        printf("子プロセスが正常終了しました %d\n", WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status))
    {
        printf("子プロセスがシグナルにより終了しました %d\n", WTERMSIG(status));
    }

    return 0;
}

int main()
{
    printf("プロセスの開始\n");
    const auto pid = fork();
    if (pid == -1)
    {
        perror("fork error\n");
        return 1;
    }

    auto ptr = std::make_shared<int>(1);
    if (pid == 0)
    {
        return ChildDo(ptr);
    }

    return ParentDo(pid, ptr);
}