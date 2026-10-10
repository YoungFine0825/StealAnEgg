# Steal An Egg

Low Poly 美术风格的多人在线偷蛋游戏。玩家进入周期性开放的动物领地偷取蛋，在动物追逐和其他玩家干扰下返回安全区，将蛋出售或放入围栏培养孵化成宠物。

> 基于 Unreal Engine 5（`EngineAssociation {554ADABB-46B5-46F1-C9FD-B7B543BCA701}`），C++ + Blueprint 混合开发，采用 Gameplay Ability System (GAS)。

## 项目定位

**Steal An Egg 是一款开源社区游戏**，对标 [SuperTuxKart](https://supertuxkart.net/) 一类的自由软件多人游戏：

- **开源开放**：全部源码公开，接受社区贡献（代码、美术、关卡、平衡性、本地化等）。
- **社区驱动**：玩法与内容由社区共同推进，而非商业发行团队闭门开发。
- **免费且无内购**：下载即玩，不设付费墙、不含商城与开箱。
- **以完整可玩为目标**：追求「下载即能开局多人对战」的自包含体验，专用服务器可自行搭建。
- **技术可复用**：使用 UE5 + GAS 的公开技术栈，期望成为社区学习 UE5 多人玩法开发的参考项目。

这与本项目采用 C++ 源码、公开 GitHub 仓库、不依赖闭源商业服务的取向一致。

## 核心玩法

- 每个专用服务器进程承载一个独立世界，单世界最多 8 名玩家。
- 动物领地每 5 分钟开放一轮，结束后进入 10 秒刷新阶段。
- 拾蛋后进入「强制持蛋」状态：不能切换快捷栏、不能用武器、不能放置捕兽夹，只会被动物追逐。
- 携带蛋越过安全区判定线即偷取成功；系统传送进入不算。
- 蛋可即时出售，或放入围栏成长 → 手动孵化 → 宠物放入围栏持续产金币。
- 用金币与跑步机提升奔跑能力，挑战更远、更稀有的领地。

## 目录结构

```
StealAnEgg/
├── Client/StealAnEgg/          UE5 客户端 + 专用服务器工程
│   ├── Source/
│   │   ├── StealAnEgg/         游戏运行时模块（业务逻辑）
│   │   │   ├── AbilitySystem/    GAS：Ability / GameEffect / AttributeSet / Task
│   │   │   ├── GameFramework/    GameInstance / PlayerController / PlayerState / GameMode / GameState
│   │   │   ├── Interaction/      交互与拾取（InteractableTarget / Pickupable / InteractionComponent）
│   │   │   ├── Inventory/        快捷栏 / 手持 / 被盗蛋 / 物品类型
│   │   │   ├── Online/           认证 / 连接 / 专服 / 服务器浏览器 子系统
│   │   │   ├── Player/           玩家角色 / 属性组件 / 手持物品表现
│   │   │   ├── Presentation/     物品外观 Actor
│   │   │   └── World/            Egg / 世界刷新周期 / 安全区
│   │   └── StealAnEggUI/       界面模块（HUD、交互 UI 配置）
│   ├── Config/                 DefaultEngine/Game/Input 等配置
│   ├── Content/                资产（Blueprints / Eggs / Levels / UI / World / DataTable）
│   └── Plugins/                GameplayMessageRouter、VibeUE
├── scripts/                    辅助脚本（当前为空）
├── logs/                       构建/运行日志（当前为空）
├── mcp.json                    MCP 服务器配置（blender、vibeue）
└── ToDoList.txt                待办事项
```

## 技术架构

- **客户端**：登录、服务器选择、输入、画面/动画/音效与 UI，向专用服务器提交操作请求，不决定金币、物品归属、攻击结果、蛋品质或孵化结果。
- **专用服务器**：世界实时状态的权威，负责移动/交互/攻击/捕兽夹判定、快捷栏与手持状态、蛋的拾取/携带/掉落/偷取成功判定、动物行为、安全区与领地管理、世界刷新、数据保存。
- **业务后端 + 持久化数据库**（规划中）：账户认证、服务器列表、会话管理、长期数据（金币/升级/快捷栏/蛋/宠物/围栏/在线状态），保证单账户不能同时进入多个世界。

### 技术栈

| 项 | 值 |
| --- | --- |
| 引擎 | Unreal Engine 5（**源码版 / Engine Source Build**，构建 Dedicated Server 所必需） |
| 语言 | C++（运行时模块）+ Blueprint / UMG |
| 玩法框架 | Gameplay Ability System（GAS）、GameplayTags、GameplayTasks |
| AI / 状态 | AIModule、NavigationSystem、StateTree、GameplayStateTree |
| UI | CommonUI（`CommonGameViewportClient`）、UMG |
| 输入 | EnhancedInput + CommonInput（键鼠 / 手柄） |
| 网络 | NetCore 复制；服务器寻址 HTTP + Json |
| 渲染 | DX12 / SM6，朝向 Mobile 硬件档（Scalable） |

运行时模块依赖见 `Source/StealAnEgg/StealAnEgg.Build.cs`，模块与插件声明见 `StealAnEgg.uproject`。

## 构建与运行

> **本项目基于源码版 Unreal Engine 5 构建。** 由于需要构建 Dedicated Server（`StealAnEggServer` Target）、发布服务端二进制并进行多平台打包，必须使用从源码编译的引擎（Engine Source Build），而非 Epic Games Launcher 安装的预编译版本。

### 前置：准备源码版引擎

1. 从 Epic Games 的 GitHub 仓库获取与你目标版本一致的 Unreal Engine 源码（需要 Epic 账号关联 GitHub）。
2. 按官方文档在本地编译引擎（`Setup.bat` → `GenerateProjectFiles.bat` → 编译 `UE5` 解决方案）。
3. 记录引擎根目录（下称 `<UE_ROOT>`），例如 `D:\UE\UnrealEngine`。

### 编译项目

1. 关联引擎：右键 `Client/StealAnEgg/StealAnEgg.uproject` → **Switch Unreal Engine version** 选择你的源码版引擎；或直接使用源码版引擎的 `GenerateProjectFiles.bat` 生成工程。
2. 生成 VS 工程文件后，用 VS 打开 `Client/StealAnEgg/StealAnEgg.sln`，编译 `StealAnEggEditor`（Development Editor）用于开发。
3. 双击 `.uproject` 打开编辑器；默认关卡为 `/Game/StealAnEgg/Levels/L_MainWorld`。

### 构建 Dedicated Server

使用源码版引擎的 `RunUAT.bat`（AutomationTool）构建服务端：

```
<UE_ROOT>\Engine\Build\BatchFiles\RunUAT.bat BuildCookRun ^
  -project="D:\Dev\StealAnEgg\Client\StealAnEgg\StealAnEgg.uproject" ^
  -noP4 -platform=Win64 -server -serverconfig=Development ^
  -cook -build -stage -pak -archive ^
  -archivedirectory="D:\Dev\StealAnEgg\Build\Server"
```

服务端启动时使用 `ServerDefaultMap=/Engine/Maps/Entry`，随后由业务后端/命令行参数指定世界地图。

Target 定义：`StealAnEgg`、`StealAnEggClient`、`StealAnEggServer`、`StealAnEggEditor`（见 `Source/*.Target.cs`）。

未安装源码版引擎的开发者可以先编译 `StealAnEggEditor` 参与客户端逻辑开发，但无法构建/打包专用服务器。


## 待办

见 `ToDoList.txt`。

## 参与贡献

欢迎社区贡献代码、美术资源、关卡、玩法平衡、文档与本地化。

- 仓库：`git@github.com:YoungFine0825/StealAnEgg.git`（分支 `main`）
- 提交流程：fork → 从 `main` 拉分支 → 提交改动 → 发起 Pull Request。
- 提交前请确保 `StealAnEggEditor` Target 能正常编译通过。
- 较大的玩法或系统改动，建议先通过 Issue 说明设计，再动手实现。

## 许可协议

作为开源社区游戏，本项目以自由软件方式发布（具体许可证待补充，建议采用 GPL / MIT 等 OSI 认可协议并加入 `LICENSE` 文件）。在许可证正式确定前，贡献内容默认视为可按上述开源方式分发。
