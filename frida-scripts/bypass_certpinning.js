Java.perform(function () {
    console.log("Starting SSL pinning bypass...");

    // Create a custom implementation of X509TrustManager
    var CustomTrustManager = Java.registerClass({
        name: 'com.example.CustomTrustManager',
        implements: [Java.use('javax.net.ssl.X509TrustManager')],
        methods: {
            checkClientTrusted: function (chain, authType) {
                console.log("[+] checkClientTrusted called");
            },
            checkServerTrusted: function (chain, authType) {
                console.log("[+] checkServerTrusted called");
            },
            getAcceptedIssuers: function () {
                console.log("[+] getAcceptedIssuers called");
                return [];
            }
        }
    });

    // Hook TrustManagerFactory to replace the default TrustManager
    var TrustManagerFactory = Java.use("javax.net.ssl.TrustManagerFactory");
    TrustManagerFactory.getTrustManagers.implementation = function () {
        console.log("[+] Replacing TrustManager");
        return [CustomTrustManager.$new()];
    };

    console.log("SSL pinning bypass applied.");
});
