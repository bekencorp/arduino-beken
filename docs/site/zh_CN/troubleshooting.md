# 故障排查

## 先回到已知基线

在排查复杂 sketch 之前，先确认下面几项是否一致：

- 预期的 `TARGET`
- 预期的 generic variant
- 当前 target 对应的 staged hardware link
- 同一 target 的导出 SDK
- 一个仓库自带示例，优先 `examples/Blink`

## `arduino-cli` 找不到平台

优先检查：

- 是否执行过 `make TARGET=<target> prepare-platform`
- `.arduino-cli-test/user/hardware/beken/<target>` 是否指向预期 staged platform
- `--fqbn` 是否与目标 target / variant 一致

## 看不到分区方案菜单

优先检查：

- `config/<target>/` 下是否真的存在多份 `auto_partitions*.csv`
- 新增文件名是否符合 `auto_partitions_<name>.csv` 或 `auto_partitions.<name>.csv`
- 新增文件后是否重新执行过 `make TARGET=<target> prepare-platform`

## 还没开始普通 sketch 编译就失败了

如果失败发生在 prebuild 阶段，先检查：

- 当前选中的 `auto_partitions*.csv` 是否存在
- 分区大小是否满足上游要求的对齐规则
- 分区布局是否重叠，或是否超出 flash 容量

## `bk_idk` 能编，`arduino-cli` 不能编

这种情况通常说明问题在这些层之一：

- staged platform 内容
- 导出 SDK 内容
- Arduino CLI build property
- 两条构建链对 sketch 的集成方式差异

先把问题收缩到仓库自带示例，再改代码。

## 外设示例能编译，但硬件行为不对

优先检查：

- 板上是否真的引出了对应引脚
- 该引脚是否已被板载其他功能占用
- 这个外设是否需要外接回环或额外接线
- 板级电压和模拟范围是否匹配外设

## 看不到串口输出

确认：

- 串口设备是否选对
- 波特率是否为 `115200`，除非示例另有说明
- 板子是否真的启动到了新刷入的镜像

## 提交 bug 时建议附带什么

建议至少附带：

- target SoC 和 variant
- 主机操作系统
- 具体执行的命令
- 是否能用仓库自带示例复现
- 最短复现路径
