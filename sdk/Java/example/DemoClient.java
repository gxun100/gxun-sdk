package com.kd100.demo.example;

import com.gxun.core.*;

import java.nio.charset.StandardCharsets;

public class DemoClient {
    private static final int TO_GID = -1;
    private static final int TO_SUB_ID = -1;
    private static final String WORKING_DIR = ".";

    private static byte[] mockBytes() {
        return "Hello,World!".getBytes(StandardCharsets.UTF_8);
    }

    public static void main(String[] args) throws GXunException {
        GXunNodeConfig config = GXunNodeConfig.newBuilder()
                //设置结点配置路径
                .setConfPath(WORKING_DIR + "/demo.conf")
                //若设置，使用这里的值为准；不设置则使用配置文件中的值
                //.setGid(GID)
                //.setSubId(SID)
                //.setListenIp(LISTENING_IP)
                .setListenPort(55555)
                .build();
        //传入结点配置参数，启动结点实例
        GXunNode node = GXunNode.create(config);
        if (node == null) {
            throw new RuntimeException("FAIL TO CREATE NODE");
        }
        GXunTunnel tunnel = node.openTunnel(TO_GID, TO_SUB_ID, TransferMode.DIRECT);
        if (tunnel == null) {
            throw new RuntimeException("FAIL TO CREATE TUNNEL");
        }
        GXunConnection connection = tunnel.openConnection();
        if (connection == null) {
            throw new RuntimeException("FAIL TO OPEN CONNECTION");
        }

        for (int i = 0; i < 10000; i++) {
            byte[] bytes = mockBytes();
            byte[] response = connection.remoteCall(bytes, false, 3000);
            if (response != null) {
                System.out.println("REMOTE CALL SUCCESS,LENGTH:" + response.length);
            } else {
                System.out.println("REMOTE CALL FAIL");
            }
        }

        //关闭连接
        connection.release();
        //关闭通道
        tunnel.close();
        //销毁结点
        node.destroy();

        System.out.println("CLIENT EXIT");
    }
}
