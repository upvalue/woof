module.exports = {
  basePath: '/docs',
  // assetPrefix: '/woof',
  async rewrites() {
    console.log('rewrites');
    return [
      { source: '/docs/_next/:path*', destination: '/docs/next/:path*'}
    ];
  },
}
